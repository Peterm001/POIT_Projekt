#include <Servo.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>

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
int temp = -1;
int previousMillis = 0;
Servo servo;

WiFiServer server(12000);
WiFiClient c;

void setup() {
  lightCal = analogRead(sensorPin);
  servo
  .attach(servoPin);

  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  IPAddress ip(192, 168, 168, 60);
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

  server.begin();

}

void loop() {

c = server.available();

if (c.connected())
{
   Serial.println("Connected to arduino!");

  while(true){
  char x[3];
  char tmp[1] = {c.read()};
  
  if (tmp[0] == '#'){
      x[0]={c.read()}; 
      x[1]={c.read()}; 
      x[2]={c.read()};
  }
  

  int currentMillis = millis();

  if (currentMillis - previousMillis >= 100){
    servo.write(atoi(x));
    previousMillis = currentMillis;
  }
  } 
}     
}

void SendData(int* value){

    lightCal = analogRead(sensorPin);
    int length = digitCount(lightCal);

    if (*value == -1){
      *value = lightCal;
    }
    else if (*value == lightCal){
      return;
    }else{
      *value = lightCal;
    }
      
      
    char buffer[length+1];
    itoa(lightCal, buffer, 10);
    buffer[length] = ',';
    c.write(buffer);
    Serial.println(buffer);
}


int digitCount(int val)
{ 
  if(val > 999)
    return 4;
  else if(val > 99)
    return 3;
  else if(val > 9)
    return 2;
  else
    return 1;
}
