
#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,0,177);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);
OneWire  ds(9);  // on pin 10

void setup(void) {
  Serial.begin(9600);
  
  
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
	  client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();

client.println("<?xml version=\"1.0\" encoding=\"UTF-8?\"><exchangerates><row>");
        client.println("<exchangerate ccy=\"RUR\" base_ccy=\"UAH\" buy=\"");
client.println(temp());
client.println(" \"/> </row></exchangerates>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}
 
float temp()  // Функция определения температуры с датчика DS18B20

 {

  byte i; byte data[10];  byte addr[8]; float celsius; 

  ds.search(addr); ds.reset(); ds.select(addr);

  ds.write(0x44,1); // start conversion, with parasite power on at the end

  delay(1000);    ds.reset();  ds.select(addr);   

  ds.write(0xBE);   // Read Scratchpad

  for ( i = 0; i < 9; i++) {   // we need 9 bytes

    data[i] = ds.read();}   

  int raw = (data[1] << 8) | data[0];

    unsigned char t_mask[4] = {0x7, 0x3, 0x1, 0x0};

    byte cfg = (data[4] & 0x60) >> 5;  raw &= ~t_mask[cfg];

  celsius = (float)raw / 16; return celsius;

 }
