#include "DHT.h"
#define DHTPIN 5 
#define DHTTYPE DHT22   // DHT 22  (AM2302)

/*

 * IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <IRremote.h>

int RECV_PIN = 6;
int RELAY_PIN = 3;
int RELAY_PIN2 = 4;
IRrecv irrecv(RECV_PIN);

decode_results results;

DHT dht(DHTPIN, DHTTYPE);
void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
    Serial.println("DHTxx test!");
 
  dht.begin();
}
int on = 0;
int on2 = 0;


  unsigned long last = millis();

void loop() {
   float h = dht.readHumidity();
  float t = dht.readTemperature();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
  }
  
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    Serial.println(results.value);
		if (results.value == 16582903)
        	{
if (millis() - last > 250) {
      on = !on;
      digitalWrite(RELAY_PIN, on ? HIGH : LOW);
      digitalWrite(13, on ? HIGH : LOW);
    }
    last = millis();   }   
		if (results.value == 16615543)
        	{
if (millis() - last > 250) {
      on2 = !on2;
      digitalWrite(RELAY_PIN2, on2 ? HIGH : LOW);
      digitalWrite(13, on2 ? HIGH : LOW);
    }
    last = millis();   }      
    
    irrecv.resume(); // Receive the next value

  }

}

