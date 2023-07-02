#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <DHT.h>
const char *ssid =  "Aesthet1c0der";      // Enter your WiFi Name
const char *pass =  "biophyakash";     // Enter your WiFi Password
#define DHTPIN D1          // GPIO Pin where the dht11 is connected
#define relay D2
#define solenoid D0
#define buzzer D4
DHT dht(DHTPIN, DHT11);
WiFiClient client; 
float h;
float t;
float leakage = 0;
int smokeSensorPin = A0; // Analog input pin for smoke sensor
unsigned int sensorValue; // Variable to store sensor reading
// Variable to store sensor voltage
 // Parts per million of gas concentration
 unsigned int outputValue;
 
void setup()
{
  Serial.begin(9600);
  pinMode(buzzer,OUTPUT);
  digitalWrite(buzzer,LOW);
  pinMode(relay,OUTPUT);
  digitalWrite(relay, LOW);
  pinMode(solenoid,OUTPUT);
  digitalWrite(solenoid, LOW);
  delay(100);
  dht.begin();
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
{
  delay(500);
  Serial.print(".");
}
  Serial.println("");
  Serial.println("WiFi connected");
}
void loop()
{
h = dht.readHumidity();     // read humidity
t = dht.readTemperature();     // read temperature
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" °C, Humidity: ");
  Serial.print(h);
  Serial.println(" %");
  Serial.println("Ammonia in ppm:");
  sensorValue = analogRead(smokeSensorPin); // Read sensor value
outputValue = map(sensorValue,0,1023,0,255);
Serial.println(outputValue);
if(outputValue>=130){
  digitalWrite(buzzer,HIGH);
  leakage=1;
}
if(outputValue<130){
 digitalWrite(buzzer,LOW);
 leakage=0;
  }
  delay(1000); // Wait for 1 second
if(h>45){
    Serial.println("Chamber Humidity is High");
    Serial.println("Compressor pump is OFF");
    Serial.println("Ammonia valve is closed");
    digitalWrite(relay, HIGH);
    digitalWrite(solenoid, HIGH);
    delay(10000);
  }
  if(h<=45){
    Serial.println("Chamber Humidity is Low");
    Serial.println("Compressor pump is ON");
    Serial.println("Ammonia valve is open");
    digitalWrite(relay, LOW);
    digitalWrite(solenoid, LOW);
    delay(10000);
  }
  if (isnan(h) || isnan(t))  
{
   Serial.println("Failed to read from DHT sensor!");
   return;
}
  unsigned long currentMillis = millis(); // grab current time
   if (WiFi.status() == WL_CONNECTED) {  //Check WiFi connection status
    HTTPClient http;                    //Declare an object of class HTTPClient
    String url = "http://electronworks.000webhostapp.com/api.php?ammonia=" + String(outputValue) + "&leakage=" + String(leakage)+ "&humidity=" + String(h) + "&temperature=" + String(t);
    Serial.println(url);
    http.begin(client, url);          //Specify request destination
    delay(5000);
    int httpCode = http.GET();            //Send the request
    if (httpCode > 0) {                   //Check the returning code
      String payload = http.getString();  //Get the request response payload
      Serial.println(payload);            //Print the response payload
    }
    http.end();  //Close connection
  }
}
