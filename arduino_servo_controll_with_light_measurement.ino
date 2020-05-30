#include <Servo.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

#ifndef STASSID
#define STASSID "Eliska2"
#define STAPSK  "Eliska2000"
#endif

const int servoPin = D6;
const int sensorPin = A0;
const char* ssid = STASSID;
const char* password = STAPSK;
int lightCal;
int incomingByte;
Servo servo;

void setup() {
  lightCal = analogRead(sensorPin);
  servo
  .attach(servoPin);

  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  IPAddress ip(192, 168, 168, 100);
  IPAddress gateway(192, 168, 168, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns(8, 8, 8, 8);
  WiFi.config(ip, gateway, subnet, dns);
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) 
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {

    lightCal = analogRead(sensorPin);
//    Serial.println("Light intensity: ");
//    Serial.println(lightCal);
    incomingByte = Serial.read();  

    left();
    right();
}

void left()
{
  for (int i = 0; i <=180; i=i+18)
    {
      servo.write(i);
      delay(200);
    }   
}

void right()
{ 
  for (int i = 180; i >=0; i=i-18)
    {
      servo.write(i);
      delay(200);
    } 
} 
