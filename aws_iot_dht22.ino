#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "secrets.h"
#include "DHT.h"
#define TIME_ZONE +2
String Date_str, Time_str, Time_format;

#define ONE_WIRE_BUS_DS18B20 D1  //ds12b20
#define DHTPIN D2        // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 11
DHT dht(DHTPIN, DHTTYPE);
#define HUMIDITY_PIN D8
 
float h ;
float t;
unsigned long lastMillis = 0;
unsigned long previousMillis = 0;
const long interval = 5000;


OneWire ds(ONE_WIRE_BUS_DS18B20);
#define TEMPERATURE_PRECISION 9
DallasTemperature ds18b20(&ds);
DeviceAddress first_temp, second_temp, third_temp;
 
#define AWS_IOT_PUBLISH_TOPIC   "esp8266/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp8266/sub"
 
WiFiClientSecure net;
 
BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);
 
PubSubClient client(net);
 
time_t now;
time_t nowish = 1510592825;
 
 
void NTPConnect(void)
{
  Serial.print("Setting time using SNTP");
  configTime(TIME_ZONE * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
  const char* Timezone = "EET-2EEST,M3.5.0/3,M10.5.0/4";
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  setenv("TZ", Timezone, 1);
  //EET-2EEST,M3.5.0/3,M10.5.0/4
  
  now = time(nullptr);
  while (now < nowish)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("done!");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}
 
 
void messageReceived(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
 
 
void connectAWS()
{
  delay(3000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println(String("Attempting to connect to SSID: ") + String(WIFI_SSID));
 
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());
 
  NTPConnect();
 
  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);
 
  client.setServer(MQTT_HOST, 8883);
  client.setCallback(messageReceived);
 
 Serial.println(MQTT_HOST);
  Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(1000);
  }
 
  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

 
  Serial.println("AWS IoT Connected!");
}
 
 
void publishMessage()
{
  connectAWS();
  StaticJsonDocument<200> doc;
  doc["time"] = millis();
  if (getHumidity() == NULL || getTemperatureDHT22() == NULL || isnan(getTemperatureDHT22()) || isnan(getHumidity()))
  {
    Serial.println("Out1");
    return;}
  doc["humidity"] = getHumidity();
  doc["temperature"] = getTemperatureDHT22();
  doc["temperature_1"] = ds18b20.getTempC(first_temp);
  doc["temperature_2"] = ds18b20.getTempC(third_temp);
  doc["temperature_3"] = ds18b20.getTempC(second_temp);
  if (doc["temperature_1"] == -127 || doc["temperature_2"] == -127 || doc["temperature_3"] == -127)
  {
    Serial.println("Out2");
    digitalWrite(D0, HIGH);
    digitalWrite(D0, LOW);
    ESP.restart();
    return;
    }
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
  Serial.println("Package sended");
  Serial.println("Shutdown");
  digitalWrite(D4, HIGH);
  digitalWrite(D6, HIGH);
  digitalWrite(D7, HIGH);
  //digitalWrite(D0, HIGH);
  digitalWrite(D5, LOW);
  //ESP.deepSleep(30e6);
  Serial.println("Switch off");
  delay(3600000);
    //delay(100000);

}

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    //if (deviceAddress[i] == 0000000000000000) {digitalWrite(D0, HIGH); getDs18b20();}
  }
}
void getDs18b20()
{
  
  Serial.println("Start");
  Serial.println("Locating devices...");
  Serial.print("Found ");
  Serial.print(ds18b20.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: ");
  if (ds18b20.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  if (!ds18b20.getAddress(second_temp, 0)) Serial.println("Unable to find address for Device 1");
  if (!ds18b20.getAddress(third_temp, 1)) Serial.println("Unable to find address for Device 2");
  if (!ds18b20.getAddress(first_temp, 2)) Serial.println("Unable to find address for Device 3");

  Serial.print("Device 0 Address: ");
  printAddress(second_temp);
  Serial.println();

  Serial.print("Device 1 Address: ");
  printAddress(third_temp);
  Serial.println();

  Serial.print("Device 2 Address: ");
  printAddress(first_temp);
  Serial.println();


  ds18b20.setResolution(third_temp, TEMPERATURE_PRECISION);
  ds18b20.setResolution(second_temp, TEMPERATURE_PRECISION);
  ds18b20.setResolution(first_temp, TEMPERATURE_PRECISION);


  Serial.print("Device 1 Resolution: ");
  Serial.print(ds18b20.getResolution(second_temp), DEC);
  Serial.println();

  Serial.print("Device 2 Resolution: ");
  Serial.print(ds18b20.getResolution(third_temp), DEC);
  Serial.println();

  Serial.print("Device 3 Resolution: ");
  Serial.print(ds18b20.getResolution(first_temp), DEC);
  Serial.println();
  ///////
} 
 
void setup()
{
  pinMode(D5, OUTPUT); // general power on board
  pinMode(D6, OUTPUT); // lighter for ds12b20
  pinMode(D4, OUTPUT); // lighter for DHT22
  pinMode(D7, OUTPUT); // new Humidity sensor
  pinMode(D0, OUTPUT); //lighter
  digitalWrite(D5, HIGH); // switch on power to board
  digitalWrite(D7, HIGH);
  digitalWrite(D4, HIGH);
  Serial.begin(115200);
  //connectAWS();
  dht.begin();


  ///////
   ds18b20.begin();
   getDs18b20();

  // locate devices on the bus

}
 
void loop()
{

  Serial.println("Start");
  digitalWrite(D5, HIGH);
  Serial.println("Switched on");
  delay(30000);
  float tempDS18B20 = getTemperatureDS18B20();
  float tempDHT22 = getTemperatureDHT22();
  float humidity = getHumidity();
  float soil=digitalRead(D7);
//
  Serial.print("Temperature DHT22: ");
  Serial.print(tempDHT22);
  Serial.println(" °C");
  if (!isnan(tempDHT22))
  {
    digitalWrite(D4, HIGH);
    delay(3000);
    digitalWrite(D4, LOW);
    }
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Soil: ");
  Serial.print(soil);
    if (!isnan(soil))
  {
    digitalWrite(D7, HIGH);
    delay(3000);
    digitalWrite(D7, LOW);
    }
//
////////////////////////ds/////////
//
  printTemperature(first_temp);
  printTemperature(third_temp);
  printTemperature(second_temp);
//  delay(10000);
  now = time(nullptr);
// 
//  if (!client.connected())
//  {
//    connectAWS();
//  }
//  else
//  {
//    client.loop();
//    if (millis() - lastMillis > 1000)
//    {
//      lastMillis = millis();
      publishMessage();
//    }
//  }

}


void printTemperature(DeviceAddress deviceAddress)
{

  float tempC = ds18b20.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);

  printAddress(deviceAddress);
  Serial.println("");

}



// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)


{
  
  Serial.print("Resolution: ");
  Serial.print(ds18b20.getResolution(deviceAddress));
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

////////////////////////
float getTemperatureDS18B20() {
  
  ds18b20.requestTemperatures();
  return ds18b20.getTempCByIndex(0);
}

float getTemperatureDHT22() {
  return dht.readTemperature();
}

float getHumidity() {
  return dht.readHumidity();
}
