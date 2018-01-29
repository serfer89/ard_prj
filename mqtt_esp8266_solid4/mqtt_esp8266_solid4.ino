
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

// Update these with values suitable for your network.

const char* ssid = "VeronikaV";
const char* password = "2412198700";
const char* mqtt_server = "192.168.0.77";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
char* message_back=0;
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
  Serial.println();
String str(payload[0],payload[2]);
char charBufVar[01];
str.toCharArray(charBufVar, 01);

switch (payload[0])
    {
    case '1':
   (payload[2] == '1' ) ? digitalWrite(02, LOW) : digitalWrite(02, HIGH);
         Serial.print("#1"); client.publish("Спальня", charBufVar);
    break;
        
    case '2':
   (payload[2] == '1') ? digitalWrite(05, LOW) : digitalWrite(05, HIGH);
             Serial.print("#2");
           break;
 
    case '3':
   (payload[2] == '1' ) ? digitalWrite(00, LOW) : digitalWrite(00, HIGH);
             Serial.print("#3");
            break;

    case '4':
   (payload[2] == '1' ) ? digitalWrite(04, LOW) : digitalWrite(04, HIGH);
                Serial.print("#4"); client.publish("Спальня", charBufVar);
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
      client.publish("Спальня/Температура", "hello world");
      // ... and resubscribe
      client.subscribe("Спальня");
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


  pinMode(05, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(04, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(00, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(02, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  
  pinMode(05, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(00, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(05, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(00, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("Спальня/Температура", msg);
  }
}
