#include <SPI.h>

#include <UIPEthernet.h>
int r;

// Enter a MAC address for your controller below.

// Newer Ethernet shields have a MAC address printed on a sticker on the shield

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// if you don't want to use DNS (and reduce your sketch size)

// use the numeric IP instead of the name for the server:

IPAddress server(192,168,0,106);  // numeric IP for Google (no DNS)

//char server[] = "www.google.com";    // name address for Google (using DNS)

 

// Set the static IP address to use if the DHCP fails to assign

IPAddress ip(192,168,0,111);

 

// Initialize the Ethernet client library

// with the IP address and port of the server 

// that you want to connect to (port 80 is default for HTTP):

EthernetClient client;



void setup() {
   digitalWrite(2, HIGH);
  delay(200);
  // initialize the digital pin as an output.
 
  pinMode(2, OUTPUT);     
 // Open serial communications and wait for port to open:

  Serial.begin(9600);


  if (Ethernet.begin(mac) == 0) {

    Serial.println("Failed to configure Ethernet using DHCP");

    // no point in carrying on, so do nothing forevermore:

    // try to congifure using IP address instead of DHCP:

    Ethernet.begin(mac, ip);

  }




}

 

void loop()

{

  
  
  // give the Ethernet shield a second to initialize:

  delay(10000);

  Serial.println("connecting...");

 

  // if you get a connection, report back via serial:

  if (client.connect(server, 80)) {

    Serial.println("connected");

    // Make a HTTP request:
//String msg = "GET /getter.php?t="+ String(temp()) +"&tin="+String(tempin())+"&h="+String(Humidity())+ "&n="+String(n)+ "&type="+String(type)+ "&vent="+String(vent);
    client.println("GET /getter.php?t=718&tin=719&h=80&n=811&type=1&vent=0");

     r++;
    Serial.println(r);
   if (r > 7){  Serial.println("Reboot..."); digitalWrite(2, LOW);}

  } 

  else {

Serial.println("Reboot...");
digitalWrite(2, LOW);

  }


  if (!client.connected()) {

Serial.println("Reboot...");
digitalWrite(2, LOW);


 

    // do nothing forevermore:


   
  }

}
