// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include "DHT.h"

#define DHTPIN_1 7     // what pin we're connected to
#define DHTPIN_2 4     // what pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht1(DHTPIN_1, DHTTYPE);
DHT dht2(DHTPIN_2, DHTTYPE);

void setup() {
  Serial.begin(9600); 
  Serial.println("DHTxx test!");
 
  dht1.begin();
    dht2.begin();
  
}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht1.readHumidity();
  float t = dht1.readTemperature();
  
    float h2 = dht2.readHumidity();
  float t2 = dht2.readTemperature();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Humidity1: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature1: "); 
    Serial.print(t);
    Serial.println(" *C");
  }
  
  if (isnan(t2) || isnan(h2)) {
    Serial.println("Failed to read from DHT_2");
  } else {
    Serial.print("Humidity2: "); 
    Serial.print(h2);
    Serial.print(" %\t");
    Serial.print("Temperature2: "); 
    Serial.print(t2);
    Serial.println(" *C");
  }
  float q;
  if(h >= h2)
  {q=h-h2;}
  else {q=h2-h;}

  if (q > 15)
  {Serial.println("AVARIA!!!");}
}
