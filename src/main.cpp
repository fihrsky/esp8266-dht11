#include <Arduino.h>
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ctime>

#define FIREBASE_HOST "https://appp-c42e2.firebaseio.com"  //Change to your Firebase RTDB project ID e.g. Your_Project_ID.firebaseio.com
#define FIREBASE_AUTH "IMR7x8n1zeTlkVkuY29XEVC33cL7zLV0KCEzc54f" //Change to your Firebase RTDB secret password
#define WIFI_SSID "home sport"
#define WIFI_PASSWORD "asdffdsa"
#define MEASURES "measures"
#define dht_pin D3
#define dht_type DHT11

long read_tick, send_tick, epoch;
const long offset = 0;

FirebaseData firebaseData;
FirebaseJson json;
DHT dht(dht_pin, dht_type);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "0.id.pool.ntp.org", offset);
String tick;

void printResult(FirebaseData &data);

void setup(){
  Serial.begin(9600);
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(D0, WAKEUP_PULLUP);
  digitalWrite(BUILTIN_LED, HIGH);
  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("  Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
  
  timeClient.begin();
  timeClient.update();
  
  epoch = timeClient.getEpochTime();
  while(epoch <= 1500000000){
    timeClient.update();
    epoch = timeClient.getEpochTime();
    Serial.print(".");
    delay(300);
  }
  tick = String(epoch);
  //read_tick = timeClient.getEpochTime();
  //Serial.println(ctime(&read_tick));


  Serial.println("------------------------------------");

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)){
    Serial.println(F("Failed to read from DHT sensor!"));
      return;
  }

  Serial.println(epoch);
  Serial.println(tick);
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));

  // Compute heat index in Celsius (isFahreheit = false)
  //float hic = dht.computeHeatIndex(t, h, false);

  digitalWrite(BUILTIN_LED, LOW);
  //Firebase.setFloat(firebaseData, "Post-1/" + tick + "/epoch", epoch);
  Firebase.setFloat(firebaseData, "Post-1/" + tick + "/temperature", t);
  Firebase.setFloat(firebaseData, "Post-1/" + tick + "/humidity", h);
  digitalWrite(BUILTIN_LED, HIGH);

  ESP.deepSleep(595*1e06);
}

void loop(){
}