#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h> 
#include "DHT.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

#define DHTPIN 5
#define DHTTYPE DHT11

boolean incoming = 0;
 
// Введите MAC-адрес и IP-адрес вашего контроллера
// IP-адрес должен соответствовать вашей локальной сети
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDA, 0x02 };
IPAddress ip(192,168,0,177); //<<< IP-АДРЕС ВВОДИТЬ СЮДА!!!
byte servert[] = { 192, 168, 0, 106 }; 
EthernetClient client;
OneWire  ds(3);
DHT dht(DHTPIN, DHTTYPE);
int Condi;
int type=1; // 1 - auto; 2 - manual
int vent=0;

// Инициализация библиотеки Ethernet server
// с тем IP-адресом и портом который мы хотим использовать
// (порт 80 по умолчанию используется для HTTP):
EthernetServer server(80);
 
void setup()
{
  pinMode(2, OUTPUT);
  pinMode(8, OUTPUT);
   pinMode(4, OUTPUT);
  // запускаем Ethernet соединение и сервер:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.begin(9600);
  dht.begin();
}
 
void loop()
{
   trans();
  
  
  // ожидание подключения клиентов
  EthernetClient client = server.available();
  if (client) {
    // Запрос HTTP заканчивается пустой строкой
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // Если будет получен символ переноса строки и
    // сама строка при этом пустая, значит
    // HTTP-запрос закончился, можно отвечать
 
    // считываем URL с символа "$" до первого пробела
        if(incoming && c == ' '){
          incoming = 0;
        }
        if(c == '$'){
          incoming = 1;
        }
 
        // Определяем что было в URL: $1 или $2
        if(incoming == 1){

          if(c == '1'){
            Serial.println("Vent - ON");
            vent = 1;
            digitalWrite(8, HIGH);
          }
          if(c == '2'){
            vent = 2;
            Serial.println("Vent - OFF");
            digitalWrite(8, LOW);
          }

if(c == '4'){
            Serial.println("Type - auto");
            type=1;
          }
if(c == '5'){
            Serial.println("Type - manual");
type=2;
          }
 
        }
 
        if (c == '\n') {
          // началась новая строка
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // в текущей строке ещё есть символы
          currentLineIsBlank = false;
        }
      }
    }
    // даём время браузеру для получения данных
    delay(1);
   
    // закрываем соединение
    client.stop();
  }
Serial.println(type);
}
void trans(){
 int n = 1;
  
String msg = "GET /getter.php?t="+ String(temp()) +"&tin="+String(tempin())+"&h="+String(Humidity())+ "&n="+String(n)+ "&type="+String(type)+ "&vent="+String(vent);

// Для отладки включим отправку данных в COM порт
Serial.println(msg);
// Если это не требуется, то можно закомментировать


client.connect(servert, 80);
client.println(msg);
//Ждем 2 сек. 
delay (10000);

//потом отключаемся
client.stop();

//Данные будут отправляться каждые 10 сек, выше мы уже подождали 2 сек, подождем еще 8 сек. 
 delay (1000);
 
  
  
}
 
int temp()  // Функция определения температуры с датчика DS18B20

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

  celsius = (int)raw / 16; 
     
  return celsius;
 }

int Humidity()
{
  int h = dht.readHumidity();
return h;
}

int tempin()
{
  int tin = dht.readTemperature();

 if(tin >= 28){
            Serial.println("Condi ON");
            Condi = 1;
            digitalWrite(2, HIGH);
          }
    else {            
      
      Serial.println("Condi OFF");
      Condi = 0;
      digitalWrite(2, LOW);}  


 if(tin <= 18){
            Serial.println("Heater ON");
            Condi = 1;
            digitalWrite(4, HIGH);
          }
    else {            
      
      Serial.println("Heater OFF");
      Condi = 0;
      digitalWrite(4, LOW);}  

return tin;
}





