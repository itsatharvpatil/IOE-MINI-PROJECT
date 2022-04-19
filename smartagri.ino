#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <DHT.h>  // Including library for dht
#include <ESP8266WiFi.h>
#define BLYNK_PRINT Serial       
#include <BlynkSimpleEsp8266.h>
 
 
#define DHTPIN D7          //pin where the dht11 is connected
DHT dht(DHTPIN, DHT11);

  
String apiKey = "XNMBOF6LK0TZ69ZM"; // Enter your Write API key from ThingSpeak
const char *ssid = "Beast's_Private";     // replace with your wifi ssid and wpa2 key
const char *pass = "atharv@2627";
const char* server = "api.thingspeak.com";
 
 
const int AirValue = 1010;   //you need to replace this value with Value_1
const int WaterValue = 500;  //you need to replace this value with Value_2
const int SensorPin = A0;
int soilMoistureValue = 0;
int soilmoisturepercent=0;
int relaypin = D4;
 
 
WiFiClient client;
 
 
 
void setup() {
  Serial.begin(9600); // open serial port, set the baud rate to 9600 bps
  pinMode(relaypin, OUTPUT);
  delay(10);
 
  dht.begin();
  
  WiFi.begin(ssid, pass);
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
    Serial.println("");
    Serial.println("WiFi connected");
    delay(4000);
}  
 
 
void loop() 
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
 
  Serial.print("Humidity: ");
  Serial.println(h);
  Serial.print("Temperature: ");
  Serial.println(t);
  
  soilMoistureValue = analogRead(SensorPin);  //put Sensor insert into soil
  Serial.println(soilMoistureValue);
  
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
 
 
if(soilmoisturepercent > 100)
{
  Serial.println("100 %");
  
  delay(250);
}
 
 
else if(soilmoisturepercent <0)
{
  Serial.println("0 %");
 
  delay(250);
  
}
 
 
else if(soilmoisturepercent >=0 && soilmoisturepercent <= 100)
{
  Serial.print(soilmoisturepercent);
  Serial.println("%");
   
  delay(250);
    
}
 
  if(soilmoisturepercent >=0 && soilmoisturepercent <= 30)
  {
    digitalWrite(relaypin, HIGH);
    Serial.println("Motor is ON");
  }
  else if (soilmoisturepercent >30 && soilmoisturepercent <= 100)
  {
    digitalWrite(relaypin, LOW);
    Serial.println("Motor is OFF");
  }
  
  if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
  {
    String postStr = apiKey;
      postStr += "&field1=";
      postStr += String(soilmoisturepercent);
      postStr += "&field2=";
      postStr += String(h);
      postStr += "&field3=";
      postStr += String(t);
      postStr += "&field4=";
      postStr += String(relaypin);  
      postStr += "\r\n\r\n\r\n\r\n";
    
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
   
  } 
    client.stop();
  
}
