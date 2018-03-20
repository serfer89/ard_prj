

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include "DHT.h"
#define DHTPIN 13     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);


const char* ssid = "veronikav";
const char* password = "2412198700";
const char* mqtt_server = "192.168.0.77";

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
WiFiClient espClient;
PubSubClient client(espClient);


char* room = "Спальня";
long lastMsg = 0;
char answ[9];
int startup = 0;
String ip;
String h_ind;

int but_1 = 16;
int but_2 = 5;
int but_3 = 4;

int led_1 = 0;
int led_2 = 2;
int led_3 = 14;

int t;
int but1_state = LOW;
int but2_state = LOW;
int but3_state = LOW;


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  answ[0] = {'o'};
  answ[1] = {'/'};
  answ[2] = (char)payload[4];// room
  answ[3] = {'/'};
  answ[4] = (char)payload[0];// dev id
  answ[5] = {'/'};
  answ[6] = (char)payload[2]; // dev state
  Serial.println();
switch (payload[0])
    {

   
         case '1':
    
  if (payload[2] == '0' ) {digitalWrite(led_1, LOW); but1_state = HIGH;} 
  else {digitalWrite(led_1, HIGH); but1_state = LOW;}
                client.publish(room, answ);
                Serial.print("#1");
        break;

    case '2':
   if (payload[2] == '0' ) { digitalWrite(led_2, LOW); but2_state = HIGH; } 
   else {digitalWrite(led_2, HIGH); but2_state = LOW;}
                client.publish(room, answ);
                Serial.print("#2");
        break;
        
    case '3':
   if (payload[2] == '0' ) { digitalWrite(led_3, LOW); but3_state = HIGH; } 
   else {digitalWrite(led_3, HIGH); but2_state = LOW;}
                client.publish(room, answ);
                Serial.print("#3");
        break;
        
 
    case '4':
   (payload[2] == '0' ) ? digitalWrite(04, LOW) : digitalWrite(04, HIGH);
                client.publish(room, answ);
                Serial.print("#4");
        break;

/*
    case '5':
   (payload[2] == '0' ) ? digitalWrite(D5, LOW) : digitalWrite(14, HIGH);
                client.publish(room, answ);
                Serial.print("#5");
        break;
    case '6':
   (payload[2] == '0' ) ? digitalWrite(D6, LOW) : digitalWrite(12, HIGH);
                client.publish(room, answ);
                Serial.print("#6");
        break;
    case '7':
    
  if (payload[2] == '0' ) {digitalWrite(led_1, LOW); but1_state = HIGH;} 
  else {digitalWrite(led_1, HIGH); but1_state = LOW;}
                client.publish(room, answ);
                Serial.print("#7");
        break;

    case '8':
   if (payload[2] == '0' ) { digitalWrite(led_2, LOW); but2_state = HIGH; } 
   else {digitalWrite(led_2, HIGH); but2_state = LOW;}
                client.publish(room, answ);
                Serial.print("#8");
        break;*/
    }

}

bool reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");

      // Once connected, publish an announcement...
      Serial.println(startup);
      client.publish("Спальня", "hello world");
      // ... and resubscribe
      client.subscribe("Спальня");
            client.publish("Спальня", "startup");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);
  pinMode(led_3, OUTPUT);

  pinMode(but_1, INPUT);
  pinMode(but_2, INPUT);
  pinMode(but_3, INPUT);


  Serial.begin(9600);
  Serial.println("DHTxx test!");
  dht.begin();
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  httpUpdater.setup(&httpServer);
  httpServer.begin();
  Serial.println(WiFi.localIP());
  ip = WiFi.localIP().toString();

  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n");


}

void loop() {

   float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

 
 
 httpServer.handleClient();


    if (Serial.available() > 0) {
    // read the incoming byte:
                t = Serial.read();
        t = t-48;
                // say what you got:
                Serial.print("I received: ");
                Serial.println(t, DEC);
                
          if (t == 1) {digitalWrite(led_1, HIGH); but1_state = LOW; Serial.println("serial - HIGH");}  
          if (t == 0) {digitalWrite(led_1, LOW); but1_state = HIGH; Serial.println("serial - LOW");}
        
          if (t == 2) {digitalWrite(led_2, HIGH); but2_state = LOW; Serial.println("seria2 - HIGH");}
          if (t == 3) {digitalWrite(led_2, LOW); but2_state = HIGH; Serial.println("seria2 - LOW");}  
        }  
  
  if (but1_state != digitalRead(but_1)) 
  {
    Serial.println(but1_state);
    Serial.println(digitalRead(but_1));



  if (digitalRead(but_1) == HIGH )
  {digitalWrite(led_1, HIGH); /*client.publish(room, "o/5/1/1");*/ but1_state = HIGH; Serial.println("btn 1 - HIGH");}
  
  if (digitalRead(but_1) == LOW )
  {digitalWrite(led_1, LOW); /*client.publish(room, "o/5/1/0");*/ but1_state = LOW; Serial.println("btn 1 - LOW");}
  }
  

    if (but2_state != digitalRead(but_2)) 
  {
    Serial.println(but2_state);
    Serial.println(digitalRead(but_2));
   
  if (digitalRead(but_2) == HIGH )
  {digitalWrite(led_2, HIGH); /*client.publish(room, "o/5/2/1");*/ but2_state = HIGH; Serial.println("btn 2 - HIGH");}
  
  if (digitalRead(but_2) == LOW )
  {digitalWrite(led_2, LOW); /*client.publish(room, "o/5/2/0");*/ but2_state = LOW; Serial.println("btn 2 - LOW");}
  }

   if (but3_state != digitalRead(but_3)) 
  {
    Serial.println(but3_state);
    Serial.println(digitalRead(but_3));


  if (digitalRead(but_3) == HIGH )
 {digitalWrite(led_3, HIGH); /*client.publish(room, "o/5/3/1");*/ but3_state = HIGH; Serial.println("btn 3 - HIGH");}
  
  if (digitalRead(but_3) == LOW )
  {digitalWrite(led_3, LOW); /*client.publish(room, "o/5/3/0");*/ but3_state = LOW; Serial.println("btn 3 - LOW");}
  }




  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);



 long now = millis();
  if (now - lastMsg > 30000) {
    lastMsg = now;
  
    client.publish("Спальня/ip",  ip.c_str());
    client.publish("Спальня/Температура", String(hic).c_str());


  }



  if (!client.connected()) {
    reconnect();
      startup= startup+1;  


  }
  
  client.loop();
}
