/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example for Getting Started with nRF24L01+ radios. 
 *
 * This is an example of how to use the RF24 class.  Write this sketch to two 
 * different nodes.  Put one of the nodes into 'transmit' mode by connecting 
 * with the serial monitor and sending a 'T'.  The ping node sends the current 
 * time to the pong node, which responds by sending the value back.  The ping 
 * node can then see how long the whole cycle took.
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include "DHT.h"
#include "LowPower.h"

#define DHTPIN 3     // what pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);
//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 

RF24 radio(9,10);

//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.  
//

// The role of the current running sketch

int Door_Led_Pin = 13; // обрати контакт для світлодіода
int Door_Sensor_Pin = 6; // контакт для давача
int val = 0; // змінна для збереження стану давача
int rele = 7;

int i;

void setup(void)
{
  //
  // Print preamble
  //
  pinMode(rele, OUTPUT);     

  Serial.begin(57600);
  
  
  
  pinMode(Door_Led_Pin, OUTPUT); // встановити Door_Led_Pin як вихід
pinMode(Door_Sensor_Pin, INPUT); // встановити Door_Sensor_Pin як вхід
  printf_begin();
  printf("\n\rRF24/examples/GettingStarted/\n\r");


  //
  // Setup and configure rf radio
  //

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  radio.setPayloadSize(8);

  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);


  //
  // Start listening
  //

  radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();
}

void loop(void)
{
  
    LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, 
                SPI_OFF, USART0_OFF, TWI_OFF);
  
  val = digitalRead(Door_Sensor_Pin); // читати Door_Sensor_Pin
  
 if (val == LOW) {
 
 if (analogRead(A0) < 35) {
 
 digitalWrite(rele, HIGH);
 
 Serial.println("light on "); 

   
 }
   
 }
 else {digitalWrite(rele, LOW); Serial.println("light off "); }

//i++ ;  


//if (i>5){
trans(); //i=0;
//}

//Serial.println(i);
    // Try again 1s later
    delay(1000);
  }
void trans()

{
  //
  // Ping out role.  Repeatedly send the current time
  //
 
    // First, stop listening so we can talk.
    radio.stopListening();

    // Take the time, and send it.  This will block until complete
    
 byte post[8];
   

post[0] = analogRead(A0);
post[1] = dht.readHumidity();
post[2] = dht.readTemperature();
post[3] = 101;

Serial.print("Humidity: "); 
    Serial.print(post[1]);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(post[2]);
    Serial.println(" *C");
    Serial.print("Light: "); 
    Serial.println(post[0]);
       Serial.print("door: "); 
    Serial.println(val);

    printf("Now sending %lu...",post);
    bool ok = radio.write( &post, sizeof(post) );
    
    if (ok)
      printf("ok...");
    else
      printf("failed.\n\r");

    // Now, continue listening
    radio.startListening();

    // Wait here until we get a response, or timeout (250ms)
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > 200 )
        timeout = true;

    // Describe the results
    if ( timeout )
    {
      printf("Failed, response timed out.\n\r");
    }
    else
    {
      // Grab the response, compare, and send to debugging spew
      unsigned long got_time;
      radio.read( &got_time, sizeof(unsigned long) );

      // Spew it
      printf("Got response %lu, round-trip delay: %lu\n\r",got_time,millis()-got_time);
    }
radio.stopListening();


}

  //
  // Pong back role.  Receive each packet, dump it out, and send it back
  //

  // Change roles
  //

 
      // Become the primary transmitter (ping out)
    


// vim:cin:ai:sts=2 sw=2 ft=cpp
