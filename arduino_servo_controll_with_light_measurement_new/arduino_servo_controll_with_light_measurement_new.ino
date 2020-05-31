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
int digitCount(int val);
void PrintDataToMonitor(int length,int lightCal,char buffer[10]);
void SendData(int* value);
Servo servo;

WiFiServer server(12000);
WiFiClient c;

void setup() {
  lightCal = analogRead(sensorPin);
  servo.attach(servoPin);

  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  IPAddress ip(192, 168, 168, 70);
  IPAddress gateway(192, 168, 168, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns(8, 8, 8, 8);
  WiFi.config(ip, gateway, subnet, dns);
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) 
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(1000);
   // ESP.restart();
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();

}

void loop() 
{
  c = server.available();
  if (c.connected())
  {
    Serial.println("Connected to arduino!");
    while(true)
    {
     char x[3];
     char tmp[1] = {c.read()};
  
     if (tmp[0] == '#'){
         x[0]={c.read()}; 
         x[1]={c.read()}; 
         x[2]={c.read()};
     }

      
      int currentMillis = millis();
      if (currentMillis - previousMillis >= 250)
      {
        servo.write(atoi(x));
        SendData(&temp);
        previousMillis = currentMillis;
      }
    } 
  }     
}

void SendData(int* value)
{
    lightCal = analogRead(sensorPin);
    int length = digitCount(lightCal);  
    char buffer[10];
    char endString[2];
    endString[0] = ',';
    endString[1] = '\0';
    snprintf(buffer, 10, "%d", lightCal);
    strcat(buffer, endString);
    //PrintDataToMonitor(length,lightCal,buffer);
    c.write(buffer);
    yield(); 
}

void PrintDataToMonitor(int length,int lightCal,char buffer[10])
{  
    Serial.println("|------------START------------------------------------------------------------------|");
    Serial.print("    LightCal: ");
    Serial.print(lightCal);
    Serial.print("    Digits: ");
    Serial.print(length);
    Serial.print("    SizeOfBuffer: ");
    Serial.print(sizeof(buffer));
    Serial.print("    SizeOfChar: ");
    Serial.print(sizeof(char));
    Serial.print("    Buffer: ");
    Serial.println(buffer);
    Serial.println("|------------END--------------------------------------------------------------------| \n\n"); 
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
