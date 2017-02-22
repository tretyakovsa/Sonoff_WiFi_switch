/*
 * Sonoff - Wi-Fi switch https://github.com/tretyakovsa/Sonoff_WiFi_switch
 * Arduino core for ESP8266 WiFi chip https://github.com/esp8266/Arduino
 * Arduino ESP8266 filesystem uploader https://github.com/esp8266/arduino-esp8266fs-plugin
 */
#include <ESP8266WiFi.h>             //Содержится в пакете
#include <ESP8266WebServer.h>        //Содержится в пакете
#include <ESP8266SSDP.h>             //Содержится в пакете
#include <FS.h>                      //Содержится в пакете
#include <time.h>                    //Содержится в пакете
#include <Ticker.h>                  //Содержится в пакете
#include <WiFiUdp.h>                 //Содержится в пакете
#include <ESP8266HTTPUpdateServer.h> //Содержится в пакете
#include <ESP8266HTTPClient.h>       //Содержится в пакете
#include <DNSServer.h>               //Содержится в пакете

#include <ArduinoJson.h>             //https://github.com/bblanchon/ArduinoJson
#include <DHT.h>                     //https://github.com/markruys/arduino-DHT
// DHT C автоматическим определением датчиков.Поддержка датчиков DHT11,DHT22, AM2302, RHT03.

// Настройки DNS сервера и адреса точки в режиме AP
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
// Web интерфейс для устройства
ESP8266WebServer HTTP(80);
//ESP8266WebServer HTTPWAN(ddnsPort);
ESP8266WebServer HTTPWAN;
ESP8266HTTPUpdateServer httpUpdater;
// Для файловой системы
File fsUploadFile;

// Для тикера
Ticker tickerSetLow;
Ticker tickerAlert;

#define TACH_PIN 0    // Кнопка управления
#define PIR_PIN 2     // RIR sensors
#define RELE1_PIN 12  // Реле
#define LED_PIN 13    // Светодиод
#define DHTPIN 14     // Pin which is connected to the DHT sensor.
DHT dht;

// Определяем строку для json config
String jsonConfig = "";

// Определяем переменные
String module[]={"sonoff"};
//,"rbg","jalousie"};

//Обшие настройки
String ssidName     = "WiFi";      // Для хранения SSID
String ssidPass = "Pass";      // Для хранения пароля сети
String ssidApName = "Sonoff";      // SSID AP точки доступа
String ssidApPass = "";        // пароль точки доступа
String ssdpName = "Sonoff";    // SSDP
// Переменные для обнаружения модулей
String Devices = "";            // Поиск IP адресов устройств в сети
String DevicesList = "";        // IP адреса устройств в сети
int timeZone = 3;               // часовой пояс GTM
String calibrationTime = "03:00:00"; // Время колибровки часов
// Переменные для таймеров
String times1 = "";             // Таймер 1
String times2 = "";             // Таймер 2
int timeSonoff = 10;            // Время работы реле
String Language ="ru";          // язык web интерфейса
String Lang ="";                // файлы языка web интерфейса
volatile int chaingtime = LOW;
volatile int chaing = LOW;
int state0 = 0;
int task = 0;
// Переменные для ddns
String ddns = "";               // url страницы тестирования WanIP
String ddnsName = "";           // адрес сайта ddns
int pirTime = 0;                // 0 = PIR off; >1 = PIR on;
int ddnsPort = 8080; // порт для обращение к устройству с wan

unsigned int localPort = 2390;
unsigned int ssdpPort = 1900;

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

void setup() {
 Serial.begin(115200);
 Serial.println("");
 dht_init();
 pinMode(RELE1_PIN, OUTPUT);
 pinMode(LED_PIN, OUTPUT);
 // Включаем работу с файловой системой
 FS_init();
 // Загружаем настройки из файла
 loadConfig();
  // Кнопка будет работать по прерыванию
 attachInterrupt(TACH_PIN, Tach_0, RISING); //прерывание сработает, когда состояние вывода изменится с низкого уровня на высокий
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
 Time_init(timeZone);
 // Будет выполняться каждую секунду проверяя будильники
 tickerAlert.attach(1, alert);
 ip_wan();
}

void loop() {
 dnsServer.processNextRequest();
 HTTP.handleClient();
 delay(1);
 HTTPWAN.handleClient();
 delay(1);
 handleUDP();

 if (chaing) {
  noInterrupts();
  state0=!state0;
  digitalWrite(RELE1_PIN,state0);
  chaing = 0;
  interrupts();
 }

 switch (task) {
  case 1:
   Time_init(timeZone);
   task = 0;
   break;
  case 2:
   ip_wan();
   task = 0;
   break;
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
 if (calibrationTime.compareTo(Time) == 0) {
  task=1;
 }
 if (pirTime > 0 && state0 == 0 && digitalRead(PIR_PIN)) {
  alarm_pir();
 }

 Time = Time.substring(3, 8); // Выделяем из строки минуты секунды
 // В 15, 30, 45 минут каждого часа идет запрос на сервер ddns
 if ((Time == "00:00" || Time == "15:00" || Time == "30:00"|| Time == "45:00") && ddns != "") {
  task=2;
 }
}

