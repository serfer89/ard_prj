#include <DHT.h>

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
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

#define DHTPIN 05    // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

// Update these with values suitable for your network.

const char* ssid = "VeronikaV";
const char* password = "2412198700";
const char* mqtt_server = "192.168.0.77";

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char mhic[50];
char* room = "Кухня";
char* room_temp="Кухня/Температура";
char answ[9];
int startup = 0;

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
                Serial.println(WiFi.localIP());

          Serial.println(WiFi.macAddress());
      // Once connected, publish an announcement...
      client.publish("Кухня/Температура", "hello world");
      // ... and resubscribe
      client.subscribe("Кухня");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.print(".");
  }

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
   if (payload[2] == '0' ) { digitalWrite(04, LOW);} 
   else {digitalWrite(04, HIGH);}
         client.publish(room, answ);
         Serial.print("#1");
    break;
 
    case '2':
   (payload[2] == '0' ) ? digitalWrite(00, LOW) : digitalWrite(00, HIGH);
             client.publish(room, answ);
             Serial.print("#3");
            break;
    }

}

void setup() {
    dht.begin();
  pinMode(04, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(00, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}





void loop() {

  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);







  long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;
    dtostrf(hic, 4, 0, mhic);

    
    Serial.print("Publish message: ");
    Serial.println(mhic);
    client.publish("Кухня/Температура", mhic);
  }  
  
   if (!client.connected()) {
    reconnect();
      startup= startup+1;  


  }

  client.loop();
}
