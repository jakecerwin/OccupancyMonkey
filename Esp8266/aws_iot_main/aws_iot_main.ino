#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "secrets.h"


#define D6T_addr 0x0A
#define D6T_cmd 0x4C
#define buflen 16

int PIRIn = D7;
int SDAIn = D4;
int SCLIn = D5;


String Status = "unoccupied";
String oldstatus = "unoccupied";
float h ;
float t;
unsigned long lastMillis = 0;
unsigned long previousMillis = 0;
const long interval = 5000;

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

//Dummy update for now

uint8_t calc_crc(uint8_t data) {
    int index;
    uint8_t temp;
    for (index = 0; index < 8; index++) {
        temp = data;
        data <<= 1;
        if (temp & 0x80) {data ^= 0x07;}
    }
    return data;
}

/** <!-- D6T_checkPEC {{{ 1--> D6T PEC(Packet Error Check) calculation.
 * calculate the data sequence,
 * from an I2C Read client address (8bit) to thermal data end.
 */
bool D6T_checkPEC(uint8_t buf[], int n) {
    int i;
    uint8_t crc = calc_crc((D6T_addr << 1) | 1);  // I2C Read address (8bit)
    for (i = 0; i < n; i++) {
        crc = calc_crc(buf[i] ^ crc);
    }
    bool ret = crc != buf[n];
    if (ret) {
        Serial.print("PEC check failed:");
        Serial.print(crc, HEX);
        Serial.print("(cal) vs ");
        Serial.print(buf[n], HEX);
        Serial.println("(get)");
    }
    return ret;
}

void setup()
{
  pinMode(PIRIn, INPUT);
  
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
uint8_t buf[buflen];
int count = 0 ;

void loop()
{

  delay(readDelay);

  memset(buf, 0, buflen);
  Wire.beginTransmission(D6T_addr);
  Wire.write(D6T_cmd);
  Wire.endTransmission();

  Serial.println("New read beginning");

  // Read
  Wire.requestFrom(D6T_addr, buflen);

  int i=0;
  while (Wire.available()) {
    Serial.print("Value:");
    Serial.println(i);
    Serial.println(Wire.read());
    //Serial.print(buf[i] + ",");
    //Serial.println("");
    //rbuf[i] = Wire.read();
    i++;
    }

  Serial.println("");

  if (D6T_checkPEC(buf, buflen - 1)) {
      return;
    }

/*
  int val = digitalRead(PIRIn);
  if (val == HIGH) {
    oldstatus = Status;
    Status = "occupied";

    count = 10;

  } else {
    if (count > 0) {
      count = count - 1;
    } else {
      oldstatus = Status;
      Status = "unoccupied";
    }
    readDelay = 1000;
  }


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
