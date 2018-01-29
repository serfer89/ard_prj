/*
 * UIPEthernet EchoServer example.
 *
 * UIPEthernet is a TCP/IP stack that can be used with a enc28j60 based
 * Ethernet-shield.
 *
 * UIPEthernet uses the fine uIP stack by Adam Dunkels <adam@sics.se>
 *
 *      -----------------
 *
 * This Hello World example sets up a server at 192.168.1.6 on port 1000.
 * Telnet here to access the service.  The uIP stack will also respond to
 * pings to test if you have successfully established a TCP connection to
 * the Arduino.
 *
 * This example was based upon uIP hello-world by Adam Dunkels <adam@sics.se>
 * Ported to the Arduino IDE by Adam Nielsen <malvineous@shikadi.net>
 * Adaption to Enc28J60 by Norbert Truchsess <norbert.truchsess@t-online.de>
 */

#include <UIPEthernet.h>
// The connection_data struct needs to be defined in an external file.
#include <UIPServer.h>
#include <UIPClient.h>
byte mac[] = { 0x54, 0x34, 0x41, 0x30, 0x30, 0x31 };                                       

IPAddress ip(192, 168, 0, 111);                        

EthernetServer server(80);



void setup() {

  Serial.begin(9600);



  // start the Ethernet connection and the server:

  Ethernet.begin(mac, ip);

  server.begin();



  Serial.print("IP Address: ");

  Serial.println(Ethernet.localIP());

}



void loop() {

  // listen for incoming clients

  EthernetClient client = server.available();



  if (client) 

  {  

    Serial.println("-> New Connection");



    // an http request ends with a blank line

    boolean currentLineIsBlank = true;



    while (client.connected()) 

    {

      if (client.available()) 

      {

        char c = client.read();



        // if you've gotten to the end of the line (received a newline

        // character) and the line is blank, the http request has ended,

        // so you can send a reply

        if (c == '\n' && currentLineIsBlank) 

        {

          client.println("<html><title>Hello World!</title><body><h3>Hello World!</h3></body>");

          break;

        }



        if (c == '\n') {

          // you're starting a new line

          currentLineIsBlank = true;

        }

        else if (c != '\r') 

        {

          // you've gotten a character on the current line

          currentLineIsBlank = false;

        }

      } 

    }



    // give the web browser time to receive the data

    delay(10);



    // close the connection:

    client.stop();

    Serial.println("   Disconnected\n");

  }

}
