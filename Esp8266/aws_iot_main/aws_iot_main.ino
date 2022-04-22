/***********************************
Occupancy Monkey (CMU ECE Capstone Project)
Authors: Jake Cerwin, Ryan Huang
Sections of Omron thermal sensor code were taken from the official Omron
devhub Github page: https://github.com/omron-devhub/d6t-2jcieev01-arduino.
These sections are indicated in the code. In addition, the following license
is provided:

MIT License
Copyright (c) 2019, 2018 - present OMRON Corporation
All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

***********************************/

#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "secrets.h"


#define D6T_addr 0x0A
#define D6T_cmd 0x4C
#define numPixels 16
#define numReads 35 //two reads per pixel to get a double, plus 3 extraneous ones
#define occupancyTemp 20.5 //room temp is around 20 degC
#define noiseTemp 35 //body temperature is around 32 degC, so if we're above 35 it's probably noise
#define unoccupiedLimit 10 //number of consecutive unoccupied readings to change status to unoccupied
#define occupiedLimit 10 //number of consecutive occupied readings to change status to occupied

String Status = "unoccupied";
String oldstatus = "unoccupied";
float h;
float t;
unsigned long lastMillis = 0;
unsigned long previousMillis = 0;
const long interval = 5000;
int occupiedCount = 0;
int unoccupiedCount = 0;

uint8_t ptatBuf[numReads];
double tempBuf[numPixels];
int pirRead;
int pirIn = D7;
bool thermalRead;

#define AWS_IOT_PUBLISH_TOPIC   "esp8266_10E_03/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp8266_10E_03/sub"

WiFiClientSecure net;

BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);

PubSubClient client(net);

time_t now;
time_t nowish = 1510592825;


void NTPConnect(void)
{
  Serial.print("Setting time using SNTP");
  configTime(TIME_ZONE * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);
  while (now < nowish)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("done!");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}


void messageReceived(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


void connectAWS()
{
  delay(3000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println(String("\n\nAttempting to connect to SSID: ") + String(WIFI_SSID));

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }

  NTPConnect();

  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);

  client.setServer(MQTT_HOST, 8883);
  client.setCallback(messageReceived);


  Serial.println("Connecting to AWS IOT");

  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(1000);
  }

  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}


void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["time"] = millis();
  doc["table_id"] = DEVICENUM;
  doc["status"] = Status;
  doc["key"] = DJANGOKEY;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

//Converts an 8-bit unsigned to a 16-bit signed -- modified from Omron Github repo
int16_t conv8us_s16_le(int n) {
    int ret;
    ret = ptatBuf[n];
    ret += ptatBuf[n + 1] << 8;
    return (int16_t)ret;
}

void getThermalReadings()
{
  memset(ptatBuf, 0, numReads);
  memset(tempBuf, 0, numPixels);
  
  //Let thermal sensor know we're about to request
  Wire.beginTransmission(D6T_addr);
  Wire.write(D6T_cmd);
  Wire.endTransmission();

  //Ask for all data points from sensor
  Wire.requestFrom(D6T_addr, numReads);

  //Readings come in PTAT (Proportional To Absolute Temperature) form
  int i=0;
  while (Wire.available()) {
    uint8_t reading = Wire.read();
    ptatBuf[i] = reading;
    i++;
  }

  //Get reading in double form and convert from PTAT->temp in degC (from Omron Github repo)
  double ptat = (double)conv8us_s16_le(0) / 10.0; //Filters out first 2 data points which aren't needed
  for (i = 0; i < numPixels; i++) {
    int16_t temp = conv8us_s16_le(2 + 2*i);
    tempBuf[i] = (double)temp / 10.0;
  }

/*
  //Output results to serial
  Serial.print("PTAT:");
  Serial.print(ptat, 1);
  Serial.print(" [degC], Temperature: ");
  for (i = 0; i < numPixels; i++) {
    Serial.print(tempBuf[i], 1);
    Serial.print(", ");
  } 
  Serial.println(" [degC]");
*/
}

void detectOccupancy()
{
  //Take average of all thermal pixels for a thermal value
  double avgTemp = 0;
  for (int i = 0; i < numPixels; i++) {
    avgTemp += tempBuf[i];
  }
  avgTemp /= numPixels;
  //Serial.println("Average temp: ");
  //Serial.println(avgTemp);

  //Compare average temp to occupancy and noise thresholds
  thermalRead = false;
  if (avgTemp >= occupancyTemp && occupancyTemp <= noiseTemp)
    thermalRead = true;

  Serial.print("Thermal: ");
  Serial.print(thermalRead);

  //Case by current status -- occupied
  if (Status == "occupied") {

    //When occupied we're only using thermal readings to confirm occupancy
    if (thermalRead)
      unoccupiedCount = 0;

    //Thermal sensor indicates no occupancy
    else {
      unoccupiedCount++;
      if (unoccupiedCount >= unoccupiedLimit) {
        oldstatus = Status;
        Status = "unoccupied";
        unoccupiedCount = 0;
      }
    }
  }

  //Case by current status -- unoccupied
  else {

    //When unoccupied, we need a PIR read too -- this one is a bool with HIGH as true
    pirRead = digitalRead(pirIn);

    Serial.print(", PIR: ");
    Serial.print(pirRead);

    //Both thermal and PIR sensors indicate occupancy
    if (thermalRead && pirRead == HIGH) {
      oldstatus = Status;
      Status = "occupied";
    }
  }

  Serial.print(", Status: ");
  Serial.println(Status);
}

void setup()
{
  pinMode(pirIn, INPUT);

  //Initialize I2C connection
  Wire.begin();
  Wire.beginTransmission(D6T_addr);  // I2C client address
  Wire.write(0x02);                  // D6T register
  Wire.write(0x00);                  // D6T register
  Wire.write(0x01);                  // D6T register
  Wire.write(0xEE);                  // D6T register
  Wire.endTransmission();            // I2C repeated start for read
  Wire.beginTransmission(D6T_addr);  // I2C client address
  Wire.write(0x05);                  // D6T register
  Wire.write(0x90);                  // D6T register
  Wire.write(0x3A);                  // D6T register
  Wire.write(0xB8);                  // D6T register
  Wire.endTransmission();            // I2C repeated start for read
  Wire.beginTransmission(D6T_addr);  // I2C client address
  Wire.write(0x03);                  // D6T register
  Wire.write(0x00);                  // D6T register
  Wire.write(0x03);                  // D6T register
  Wire.write(0x8B);                  // D6T register
  Wire.endTransmission();            // I2C repeated start for read
  Wire.beginTransmission(D6T_addr);  // I2C client address
  Wire.write(0x03);                  // D6T register
  Wire.write(0x00);                  // D6T register
  Wire.write(0x07);                  // D6T register
  Wire.write(0x97);                  // D6T register
  Wire.endTransmission();            // I2C repeated start for read
  Wire.beginTransmission(D6T_addr);  // I2C client address
  Wire.write(0x02);                  // D6T register
  Wire.write(0x00);                  // D6T register
  Wire.write(0x00);                  // D6T register
  Wire.write(0xE9);                  // D6T register
  Wire.endTransmission();            // I2C repeated start for read
    
  Serial.begin(115200);
  Serial.flush();
  //connectAWS();

}

int readDelay = 1000;
int mintimeBetweenSendingMessages = 5000;
int count = 0 ;

void loop()
{

  delay(readDelay);

  getThermalReadings();
  detectOccupancy();

/*
  if (oldstatus != Status) {
    if (!client.connected())
    {
      connectAWS();
      publishMessage();
    }
    else
    {
      client.loop();
      publishMessage();
    }
  }
  
  Serial.print(F("   Count: #"));
  Serial.print(count);
  Serial.print(F("  Status: "));
  Serial.println(Status);

*/





}
