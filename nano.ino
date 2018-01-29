#include <SPI.h>
#include <UIPEthernet.h>
#include "DHT.h"
// Set your own MAC address for the Ethernet module here:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// Set your own IP address for the Ethernet module here:
IPAddress ip(192,168,0,36);
EthernetServer server(80);
#define DHTPIN 7
  
// You can access your Arduino server at http://192.168.0.36
#define DHTTYPE DHT22   // DHT 22  (AM2302)
void setup()
{
  // Start Ethernet connection and web server
  Serial.println("DHTxx test!");
  dht.begin();
  Ethernet.begin(mac, ip);
  server.begin();
}

void loop() 
{


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


  // Listen for incoming connections
  EthernetClient client = server.available();
  long time = millis();
  
  if (client) // If a user is connected
  {
    // An HTTP request from the client ends with two end
    // of line characters (\n x2). We'll need code to check
    // for that.
    boolean currentLineIsBlank = true;
    // This flag is used to check if the first end of line character
    // was sent by the client.
    
    while (client.connected()) // While web client is connected
    {
      if (client.available())
      {
        char c = client.read();

        // Received two end of line characters (\n). Client has finished
        // sending the request. 
        if (c == '\n' && currentLineIsBlank) 
        {
          // Send a standard HTTP response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // Close the connection once the page is fully loaded
	  client.println("Refresh: 2");  // Refresh the page automatically every 2 seconds
          client.println(); // End of HTTP Response header
          
          // Send HTML page
          client.println("<!DOCTYPE HTML>"); // Start of HTML document
          client.println("<html>"); // Start of HTML code
          client.println("<head><title>Hello World!</title></head>"); // Set page title
          client.println("<body>"); // Start of HTML body
          client.println("<h1>Hello World!</h1>"); // Set header

          // Output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) 
          {
            int sensorReading = analogRead(analogChannel);
            client.print("Analog Input ");
            client.print(analogChannel);
            client.print(": ");
            client.print(sensorReading);
            client.println("<br />");       
          }

          // Display server uptime
          client.println("<p>Server has been running for ");
          client.println(time);
          client.println(" ms.</p>");
          client.println("</body></html>"); // End of body and HTML page
          
          break; // End of transmission
        }
        
        // If client sent one end of line character (\n),
        // raise the flag and see if the next character is another
        // end of line character
        if (c == '\n')
        {
          currentLineIsBlank = true;
        } 
        // If client sends a character that isn't an end of line character,
        // reset the flag and wait for the end of line character.
        else if (c != '\r')
        {
          currentLineIsBlank = false;
        }
      }
    }
    // Give the web browser time to receive the data
    delay(1);
    
    // Close the connection
    client.stop();
  }
}
