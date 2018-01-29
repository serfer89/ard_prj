
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
#define ONE_WIRE_BUS 05

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
// Update these with values suitable for your network.

const char* ssid = "VeronikaV";
const char* password = "2412198700";
const char* mqtt_server = "192.168.0.77";

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char* room = "Спальня";
char* room_temp="Спальня/Температура";
char msg[50];
char answ[9];
int value = 0;
int startup = 0;
float temp = 0;
long previousMillis = 0; 
int val=0; 
boolean flag=0;
int state = 0;
int con_try  = 0;
unsigned long now;
unsigned long recon_now;




void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    con_try = con_try + 1;
    Serial.print(".");
    if (con_try > 50){break;}
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
   if (payload[2] == '0' ) { digitalWrite(02, LOW);} 
   else {digitalWrite(02, HIGH);}
         client.publish(room, answ);
         Serial.print("#1");
    break;
        
    case '2':
   (payload[2] == '0') ? digitalWrite(D0, LOW) : digitalWrite(D0, HIGH);
             client.publish(room, answ);
             Serial.print("#2");
           break;
 
    case '3':
   (payload[2] == '0' ) ? digitalWrite(00, LOW) : digitalWrite(00, HIGH);
             client.publish(room, answ);
             Serial.print("#3");
            break;

    case '4':
   (payload[2] == '0' ) ? digitalWrite(04, LOW) : digitalWrite(04, HIGH);
                client.publish(room, answ);
                Serial.print("#4");
        break;


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
   (payload[2] == '0' ) ? digitalWrite(D7, LOW) : digitalWrite(13, HIGH);
                client.publish(room, answ);
                Serial.print("#7");
        break;

    case '8':
   (payload[2] == '0' ) ? digitalWrite(D8, LOW) : digitalWrite(15, HIGH);
                client.publish(room, answ);
                Serial.print("#8");
        break;
    }

}

void reconnect() {
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
      client.publish("Спальня/Температура", "hello world");
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

  pinMode(D0, OUTPUT);
  pinMode(04, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(00, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(02, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  
  pinMode(D5, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(D6, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(D7, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(D8, OUTPUT);     // Initialize the BUILTIN_LED pin as an output


  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(05, INPUT);
  sensors.begin();

  httpUpdater.setup(&httpServer);
  httpServer.begin();
  Serial.println(WiFi.localIP());

  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n");


}



void loop() {
  
   httpServer.handleClient();

  int sensorValue = analogRead(A0);
  float voltage = sensorValue * (5.0 / 1023.0);
  if (voltage > 1) {now = millis(); val++;}
  if (voltage == 0)
  {
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
    }
    
 
  if (now - lastMsg > 3000) {
    lastMsg = now;

  /*
    if(analogRead(A0)>1)//если кнопка нажата ... 
     
     {  digitalWrite(02,!digitalRead(02));
    state = digitalRead(02); //o/2/1/1
      Serial.print("state: ");
      Serial.println(state);
      Serial.println(analogRead(A0));
      String sv = "o/2/1/"+ String(state);// operation ok/id_room/id_dev/state
     client.publish(room, sv.c_str(),TRUE);
     }
*/
     
    sensors.setResolution(12);
    sensors.requestTemperatures(); // Send the command to get temperatures
    temp = sensors.getTempCByIndex(0);
    Serial.println(temp);
    client.publish(room_temp,  String(temp).c_str(),TRUE);
    
  }
    
  if (!client.connected()) {
          reconnect();
  startup= startup+1;  
      Serial.println("Startup"+startup);


  }
  client.loop();
}
