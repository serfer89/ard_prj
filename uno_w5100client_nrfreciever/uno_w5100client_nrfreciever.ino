#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "DHT.h"
#include "printf.h"
#include <Ethernet.h>
#include <OneWire.h> 

#include "DHT.h"

#define DHTPIN 3     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);
int t;
boolean incoming = 0;
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDA, 0x02 };
IPAddress ip(192, 168, 0, 177); 
byte servert[] = { 192, 168, 0, 77 }; 
EthernetClient client;
RF24 radio(9,8);


  int tin;
  int h;
  int nom;
  int light;


const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

void setup(void)
{
  
   digitalWrite(8, LOW);
     delay(200);

 
  pinMode(8, OUTPUT);  
  Serial.begin(57600);
  
  Serial.println("RF24/examples/GettingStarted/trans");
  
  radio.begin();

  radio.setRetries(15,15);


  radio.setPayloadSize(8);

    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);

  radio.startListening();

  radio.printDetails();


  Ethernet.begin(mac, ip);


  Serial.println(Ethernet.localIP());
   dht.begin();
}

void loop(void)
{
  
   Serial.println("STEP1");
  
   if ( radio.available() )
    {
      byte post[8];
      bool done = false;
      while (!done)
      {
        done = radio.read( &post, sizeof(post) );
        
  int h_server = dht.readHumidity();
  int t_server = dht.readTemperature();
        t=t_server;// временно температура сервака!!!

 Serial.println("STEP2");
 
 
     Serial.print("Humidity_server: "); 
    Serial.print(h_server);
    Serial.print(" %\t");
    Serial.print("Temperature_server: "); 
    Serial.print(t_server);
    Serial.println(" *C");
 
 Serial.print("Humidity: "); 
    Serial.print(post[1]);
 
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(post[2]);
   
    Serial.println(" *C");
    Serial.print("Light: "); 
    Serial.print(post[0]);
    Serial.println(" %");
    Serial.print("Id: "); 
    Serial.print(post[3]);
    
  tin = post[2];

  
  h = post[1];
  nom = post[3];
  light = post[0];
  
  

        printf("Got payload %lu...",post);

	
	delay(20);
      }

Serial.println("STEP3");
      radio.stopListening();

      radio.write( &post, sizeof(post) );
      Serial.println("Sent response.\n\r");

      radio.startListening();
    }

 
      radio.openWritingPipe(pipes[1]);
      radio.openReadingPipe(1,pipes[0]);
 radio.printDetails();
digitalWrite(8, HIGH);

 Serial.println("Start");

  radio.printDetails();

  Serial.println("Stop");

    delay(2000);

 Serial.println("TRY DO getter.php?t="+String(t) +"&tin="+String(tin)+"&h="+String(h)+"&n="+String(nom)+"&type=33&vent=1&light="+String(light)+"" );

  Serial.println("connecting...");


  


  if (client.connect(servert, 80)) {

    Serial.println("connected");

    client.println("GET /getter.php?t="+String(t) +"&tin="+String(tin)+"&h="+String(h)+"&n="+String(nom)+"&type=33&vent=1&light="+String(light)+"");

Serial.println("getter.php?t="+String(t) +"&tin="+String(tin)+"&h="+String(h)+"&n="+String(nom)+"&type=33&vent=1&light="+String(light)+"" );

client.stop();  
}
  else {Serial.println("not connected");}
delay(6000);
  
  }
