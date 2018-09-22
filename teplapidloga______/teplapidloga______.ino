


/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 16
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
// Update these with values suitable for your network.

const char* ssid = "veronikav";
const char* password = "2412198700";
const char* mqtt_server = "192.168.0.77";

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
char answ[9];
int value = 0;
int startup = 0;
float temp = 0;
long previousMillis = 0; 
int val=0; 
boolean flag=0;
char* room = "Коридор";
char* room_temp="Коридор/Температура";
unsigned long now;
int state = 1;
int maxTemp = 25;
int minTemp = 22;
String ip;
long lastReconnectAttempt = 0;

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
  sensors.begin();

    sensors.setResolution(12);
    sensors.requestTemperatures(); // Send the command to get temperatures
    temp = sensors.getTempCByIndex(0);

    
    Serial.println(temp);
    if (temp > maxTemp && state == 1) {Serial.println("OFF"); state=0; digitalWrite(D1, LOW);
}
    if (temp < minTemp && state == 0) {Serial.println("ON"); state=1; digitalWrite(D1, HIGH);
}

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
   if (payload[2] == '0' ) { digitalWrite(D1, HIGH); state= 0;} 
   else {digitalWrite(D1, LOW); state= 1;}
         client.publish(room, answ);
         Serial.print("#1");
    break;
        
    case 'n':
   if (payload[2] == '0' ) { maxTemp = 26; minTemp = 19;  client.publish(room, "Night mode - on");
} 
   else {int maxTemp = 26; int minTemp = 23;    
   client.publish(room, "Night mode - off");
}
             Serial.print("#2");
           break;
 /*
    case '3':
   (payload[2] == '0' ) ? digitalWrite(4, LOW) : digitalWrite(4, HIGH);
             client.publish(room, answ);
             Serial.print("#3");
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
      client.publish("Коридор/Температура", "hello world");
      // ... and resubscribe
      client.subscribe("Коридор");
            client.publish("Коридор", "startup");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      sensors.begin();

     sensors.setResolution(12);
    sensors.requestTemperatures(); // Send the command to get temperatures
    temp = sensors.getTempCByIndex(0);

    
    Serial.println(temp);
    if (temp > maxTemp && state == 1) {Serial.println("OFF"); state=0; digitalWrite(D1, LOW);
}
    if (temp < maxTemp && state == 0) {Serial.println("ON"); state=1; digitalWrite(D1, HIGH);
}
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void setup() {

  pinMode(D1, OUTPUT);
  //pinMode(5, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  sensors.begin();

  httpUpdater.setup(&httpServer);
  httpServer.begin();
  Serial.println(WiFi.localIP());
  ip = WiFi.localIP().toString();

  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n");

}


void loop() {
    httpServer.handleClient();



    

  long now = millis();
  if (now - lastMsg > 30000) {
    lastMsg = now;
  
  /*  if(analogRead(A0)>1)//если кнопка нажата ... 
     {  digitalWrite(02,!digitalRead(02));
    int state = digitalRead(02); //s/2/1/1
      Serial.print("state: ");
      Serial.println(state);
      
String sv = "s/2/1/"+ String(state);// dev state     
     client.publish("Спальня", sv.c_str(),TRUE);
     }*/

     
    sensors.setResolution(12);
    sensors.requestTemperatures(); // Send the command to get temperatures
    temp = sensors.getTempCByIndex(0);

    
    Serial.println(temp);
    if (temp > maxTemp && state == 1) {Serial.println("OFF"); state=0;
        client.publish("Коридор", "1/0/5");
}
    if (temp < minTemp && state == 0) {Serial.println("ON"); state=1;
        client.publish("Коридор",  "1/1/5");
}
    client.publish("Коридор/Температура",  String(temp).c_str(),TRUE);
    client.publish("Коридор/Підлога/ip",  ip.c_str(),TRUE);
    client.publish("Коридор/Підлога/state",  String(state).c_str(),TRUE);
    String  msg_comb="minTemp-"+String(minTemp);
    client.publish("Коридор/Підлога/ip",  msg_comb.c_str(),TRUE);


  }
    
  if (!client.connected()) {
    reconnect();
      startup= startup+1;  


  }



  /* 
  Serial.println("state "+digitalRead(A0));
  if (digitalRead(A0) == HIGH)
  {
    digitalWrite(02,!digitalRead(02));
    }
  if (val >= 500 && val <= 2000)
  {Serial.println("stage - 1 "+now); 
  digitalWrite(02,!digitalRead(02));
  state = digitalRead(02);
  String sv = "o/2/1/"+ String(state);// operation ok/id_room/id_dev/state
  client.publish(room, sv.c_str(),TRUE);
  }
      if (val >= 2000 && val <= 4000)
  {Serial.println("stage - 2 "+now);
  digitalWrite(D0,!digitalRead(D0));
  state = digitalRead(D0);
  String sv = "o/2/2/"+ String(state);// operation ok/id_room/id_dev/state
  client.publish(room, sv.c_str(),TRUE);
  }
        if (val >= 4000 )
  {Serial.println("stage - 3 "+now); Serial.println(val);}
  val = 0;
  now = 0;
    }*/
  
  client.loop();
//BTN end
}
