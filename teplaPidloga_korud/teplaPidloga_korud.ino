// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 14
#define TEMPERATURE_PRECISION 9
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress korudor_temp, wcBig_temp, wcLittle_temp;

const char* ssid = "veronikav";
const char* password = "2412198700";
const char* mqtt_server = "192.168.0.77";
String ip;
char* room = "Коридор";
char* room_temp="Коридор/Температура";
float minTemp=26.00;
float maxTemp=28.00;
int state = 1;
unsigned long now;
long lastMsg = 0;
char answ[9];


ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
WiFiClient espClient;
PubSubClient client(espClient);



void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

int i=0;

  while (WiFi.status() != WL_CONNECTED && i<100) {
    delay(500);
    Serial.print(".");
 
//    if (temp > maxTemp && state == 1) 
//    {
//      Serial.println("OFF"); state=0; digitalWrite(D1, LOW);
//    }
//    
//    if (temp < minTemp && state == 0) 
//    {
//      Serial.println("ON"); state=1; digitalWrite(D1, HIGH);
//    }
   i++;
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
   if (payload[2] == '0' ) { digitalWrite(5, HIGH); state= 0;}  //Korudor
   else {digitalWrite(5, LOW); state= 1;}
         client.publish(room, answ);
         Serial.print("#1");
    break;


        case '2':
   if (payload[2] == '0' ) { digitalWrite(4, HIGH); state= 0;}  // WC_big
   else {digitalWrite(4, LOW); state= 1;}
         client.publish(room, answ);
         Serial.print("#1");
    break;



        case '3':
   if (payload[2] == '0' ) { digitalWrite(0, HIGH); state= 0;} //WC_little
   else {digitalWrite(0, LOW); state= 1;}
         client.publish(room, answ);
         Serial.print("#1");
    break;
//
//        case '3':
//   if (payload[2] == '0' ) { digitalWrite(2, HIGH); state= 0;} //WC_little
//   else {digitalWrite(2, LOW); state= 1;}
//         client.publish(room, answ);
//         Serial.print("#1");
//    break;
//        
//    case 'n':
//   if (payload[2] == '0' ) { maxTemp = 26; minTemp = 19;  client.publish(room, "Night mode - on");
//} 
//   else {int maxTemp = 26; int minTemp = 23;    
//   client.publish(room, "Night mode - off");
//}
//             Serial.print("#2");
//           break;
 /*
    case '3':
   (payload[2] == '0' ) ? digitalWrite(4, LOW) : digitalWrite(4, HIGH);
             client.publish(room, answ);
             Serial.print("#3");
            break;*/
    }

}



void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
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
      //Serial.println(startup);
      client.publish("Коридор/Температура", "hello world");
      // ... and resubscribe
      client.subscribe("Коридор");
            client.publish("Коридор", "startup");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void setup(void)
{
  
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");


  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  httpUpdater.setup(&httpServer);
  httpServer.begin();
  Serial.println(WiFi.localIP());
  ip = WiFi.localIP().toString();

  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n");






  // Start up the library
  sensors.begin();

  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  if (!sensors.getAddress(korudor_temp, 0)) Serial.println("Unable to find address for Device 1");
  if (!sensors.getAddress(wcBig_temp, 1)) Serial.println("Unable to find address for Device 2");
  if (!sensors.getAddress(wcLittle_temp, 2)) Serial.println("Unable to find address for Device 3");

  Serial.print("Device 0 Address: ");
  printAddress(korudor_temp);
  Serial.println();

  Serial.print("Device 1 Address: ");
  printAddress(wcBig_temp);
  Serial.println();

  Serial.print("Device 2 Address: ");
  printAddress(wcLittle_temp);
  Serial.println();


  sensors.setResolution(korudor_temp, TEMPERATURE_PRECISION);
  sensors.setResolution(wcBig_temp, TEMPERATURE_PRECISION);
  sensors.setResolution(wcLittle_temp, TEMPERATURE_PRECISION);


  Serial.print("Device 1 Resolution: ");
  Serial.print(sensors.getResolution(korudor_temp), DEC);
  Serial.println();

  Serial.print("Device 2 Resolution: ");
  Serial.print(sensors.getResolution(wcBig_temp), DEC);
  Serial.println();

  Serial.print("Device 3 Resolution: ");
  Serial.print(sensors.getResolution(wcLittle_temp), DEC);
  Serial.println();
}


void mesureTemp(DeviceAddress deviceAddress)

{
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC > maxTemp){Serial.println("Off");} 
    if (tempC < minTemp){Serial.println("On");} 

}


void printTemperature(DeviceAddress deviceAddress)
{
  if (deviceAddress == wcLittle_temp){Serial.println("You are wright!");}

  float tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);

  mesureTemp(deviceAddress);
  printAddress(deviceAddress);
  Serial.println("");

}




void pushData(DeviceAddress deviceAddress)
{
      float tempC = sensors.getTempC(deviceAddress);


    if (deviceAddress == korudor_temp)
    {
    client.publish("Коридор/Температура",  String(tempC).c_str(),TRUE);
    client.publish("Коридор/Підлога/ip",  ip.c_str(),TRUE);
    }
   if (deviceAddress == wcBig_temp)
   {
    client.publish("СанВузолВеликий/Температура",  String(tempC).c_str(),TRUE);
    client.publish("СанВузолВеликий/Підлога/ip",  ip.c_str(),TRUE);
    
    }
    if (deviceAddress == wcLittle_temp)
    {
    client.publish("СанВузолМалий/Температура",  String(tempC).c_str(),TRUE);
    client.publish("СанВузолМалий/Підлога/ip",  ip.c_str(),TRUE);
    }
}

// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)


{
  
  Serial.print("Resolution: ");
  Serial.print(sensors.getResolution(deviceAddress));
  Serial.println();
}

// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  Serial.print("Device Address: ");
  printAddress(deviceAddress);
  Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}

void loop(void)
{

  httpServer.handleClient();
  long now = millis();
  if (now - lastMsg > 30000) {
    lastMsg = now;
  
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.println("DONE");

  // print the device information
//  printData(korudor_temp);
//  printData(wcBig_temp);
//  printData(wcLittle_temp);
  printTemperature(wcLittle_temp);
  printTemperature(wcBig_temp);
  printTemperature(korudor_temp);



  
//  mesureTemp(wcLittle_temp);
  
//  pushDataWcLittle(wcLittle_temp);
//  pushDataWcBig(wcBig_temp);
//  pushDataKorud(korudor_temp);
//  

pushData(wcLittle_temp);
pushData(wcBig_temp);
pushData(korudor_temp);

  
  }
    if (!client.connected()) {
    reconnect();
      //startup= startup+1;  

  }
    client.loop();
}
