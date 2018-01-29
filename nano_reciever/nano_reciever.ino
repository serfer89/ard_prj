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

#include "DHT.h"
#include "printf.h"
#include <Ethernet.h>
#include <OneWire.h> 
int t;
int h;

boolean incoming = 0;
 
// Введите MAC-адрес и IP-адрес вашего контроллера
// IP-адрес должен соответствовать вашей локальной сети
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDA, 0x02 };
IPAddress ip(192,168,0,177); //<<< IP-АДРЕС ВВОДИТЬ СЮДА!!!
byte servert[] = { 192, 168, 0, 106 }; 
EthernetClient client;

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

RF24 radio(9,8);

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


void setup(void)
{
  //
  // Print preamble
  //
   digitalWrite(8, LOW);
     delay(200);
  // initialize the digital pin as an output.
 
  pinMode(8, OUTPUT);  
  Serial.begin(57600);
  
  Serial.println("RF24/examples/GettingStarted/trans");

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


  Ethernet.begin(mac, ip);


  Serial.println(Ethernet.localIP());
}

void loop(void)
{
  //
  // Ping out role.  Repeatedly send the current time
  //

    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      byte post[8];
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &post, sizeof(post) );

        
Serial.print("Humidity: "); 
    Serial.print(post[1]);
    int h = post[1];
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(post[2]);
    int t = post[2];
    Serial.println(" *C");
    Serial.print("Light: "); 
    Serial.print(post[0]);
    Serial.println(" %");
    Serial.print("Id: "); 
    Serial.print(post[3]);

        // Spew it
        printf("Got payload %lu...",post);

	// Delay just a little bit to let the other unit
	// make the transition to receiver
	delay(20);
      }

      // First, stop listening so we can talk
      radio.stopListening();

      // Send the final one back.
      radio.write( &post, sizeof(post) );
      Serial.println("Sent response.\n\r");

      // Now, resume listening so we catch the next packets.
      radio.startListening();
    }
  

  //
  // Change roles
  //

      // Become the primary receiver (pong back)
 
      radio.openWritingPipe(pipes[1]);
      radio.openReadingPipe(1,pipes[0]);

digitalWrite(8, HIGH);

 Serial.println("Start");

  radio.printDetails();

  Serial.println("Stop");

    // Try again 1s later
    delay(2000);

 
  Serial.println("connecting...");

 

  // if you get a connection, report back via serial:

  if (client.connect(servert, 80)) {

    Serial.println("connected");

    // Make a HTTP request:
//String msg = "GET /getter.php?t="+ String(temp()) +"&tin="+String(tempin())+"&h="+String(Humidity())+ "&n="+String(n)+ "&type="+String(type)+ "&vent="+String(vent);
    client.println("GET /getter.php?t="+String(t) +"&tin=211&h="+String(h)+"&n=8441&type=4&vent=5");
  }
  else {Serial.println("not connected");}
delay(6000);
  








  }

  //
  // Pong back role.  Receive each packet, dump it out, and send it back
  //

  // Change roles
  //

 
      // Become the primary transmitter (ping out)
    


// vim:cin:ai:sts=2 sw=2 ft=cpp
