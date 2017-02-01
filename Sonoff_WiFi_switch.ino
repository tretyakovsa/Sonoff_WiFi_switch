/*
 * Sonoff - Wi-Fi switch
 */
#include <ESP8266WiFi.h>        //Содержится в пакете
#include <ESP8266WebServer.h>   //Содержится в пакете
#include <ESP8266SSDP.h>        //Содержится в пакете
#include <FS.h>                 //Содержится в пакете
#include <time.h>               //Содержится в пакете
#include <Ticker.h>             //Содержится в пакете
#include <WiFiUdp.h>            //Содержится в пакете
#include <ESP8266HTTPUpdateServer.h> //Содержится в пакете
#include <ESP8266HTTPClient.h>
#include <DNSServer.h>
#include <ArduinoJson.h>

#include <DHT.h> // http://homes-smart.ru/upload/arduino/DHTAUTO.zip DHT C автоматическим определением датчиков.Поддержка датчиков DHT11,DHT22, AM2302, RHT03.


const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
int httpPort = 80;
// Web интерфейс для устройства
ESP8266WebServer HTTP(80);
ESP8266HTTPUpdateServer httpUpdater;
// Для файловой системы
File fsUploadFile;

// Для тикера
Ticker tickerSetLow;
Ticker tickerAlert;

// Кнопка управления
#define Tach0 0

// Реле на ногах
#define rele1 12
// Светодиод на ноге
#define led 13

// DHT на ноге
#define DHTPIN            14         // Pin which is connected to the DHT sensor.
DHT dht;
// Определяем переменные

// Определяем строку для json config
String jsonConfig = "";
// Количество модулей в устройстве
int a = 1;
//char* module[] = {"rgb", "test"}; //Может ты хотел такую конструкцию сделать? (https://forum.arduino.cc/index.php?topic=424342.0)
String module[]={"sonoff"};
//,"rbg","jalousie"};
//Обшие настройки
String _ssid     = "WiFi"; // Для хранения SSID
String _password = "Pass"; // Для хранения пароля сети
String _ssidAP = "Sonoff";   // SSID AP точки доступа
String _passwordAP = ""; // пароль точки доступа
String SSDP_Name = "Sonoff";      // SSDP
// Переменные для обнаружения модулей
String Devices = "";            // Поиск IP адресов устройств в сети
String DevicesList = "";            // IP адреса устройств в сети
int timezone = 3;        // часовой пояс GTM
String kolibrTime = "03:00:00"; // Время колибровки часов
// Переменные для таймеров
String times1 = "";             // Таймер 1
String times2 = "";             // Таймер 2
int Timesonoff = 10;  // Время работы реле
String Language ="ru";  // язык web интерфейса
String Lang ="";  // файлы языка web интерфейса
volatile int chaingtime = LOW;
volatile int chaing = LOW;
int state0 = 0;
// Переменные для DDNS
String DDNS = "";               // url страницы тестирования WanIP
String DDNSName = "";           // адрес сайта DDNS

unsigned int localPort = 2390;
unsigned int ssdpPort = 1900;

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

void setup() {
 Serial.begin(115200);
 Serial.println("");
 dht_init();
 pinMode(rele1, OUTPUT);
 pinMode(led, OUTPUT);
 // Включаем работу с файловой системой
 FS_init();
 // Загружаем настройки из файла
 loadConfig();
 // Кнопка будет работать по прерыванию
 attachInterrupt(Tach0, Tach_0, RISING); //прерывание сработает, когда состояние вывода изменится с низкого уровня на высокий
 //Запускаем WIFI
 WIFIAP_Client();
 // Закускаем UDP
 udp.begin(localPort);
 //настраиваем HTTP интерфейс
 HTTP_init();
 //Serial.println("HTTP Ready!");
 //запускаем SSDP сервис
 SSDP_init();
 //Serial.println("SSDP Ready!");
 // Включаем время из сети
 Time_init(timezone);
 // Будет выполняться каждую секунду проверяя будильники
 tickerAlert.attach(1, alert);
 ip_wan();
}

void loop() {
 dnsServer.processNextRequest();
 HTTP.handleClient();
 delay(1);
 handleUDP();

 if (chaing) {
  noInterrupts();
    state0=!state0;
    digitalWrite(rele1,state0);
    chaing = 0;
    interrupts();
 }

 if (chaingtime) {
  Time_init(timezone);
  chaingtime = 0;
 }
 if (chaingtime) {
  Time_init(timezone);
  chaingtime=0;
 }
}

// Вызывается каждую секунду в обход основного циклу.
void alert() {
 String Time=XmlTime();
 if (times1.compareTo(Time) == 0) {
  Serial.println("timer1");
  Time01();
 }
 if (times2.compareTo(Time) == 0) {
  Serial.println("timer2");
  Time01();
 }
 Time = Time.substring(3, 8); // Выделяем из строки минуты секунды
 // В 15, 30, 45 минут каждого часа идет запрос на сервер DDNS
 if ((Time == "00:00" || Time == "15:00" || Time == "30:00"|| Time == "45:00") && DDNS != "") {
  ip_wan();
 }
 if (kolibrTime.compareTo(Time) == 0) {
  chaingtime=1;
 }
}

