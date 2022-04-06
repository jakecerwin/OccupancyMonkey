#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "secrets.h"
#include "DHT.h"

int inputPin = D7;


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



void setup()
{
  pinMode(inputPin, INPUT);
  Serial.begin(115200);
  connectAWS();

}

int readDelay = 1000;
int mintimeBetweenSendingMessages = 5000;

int count = 0 ;

void loop()
{

  delay(readDelay);

  int val = digitalRead(inputPin);
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







}
