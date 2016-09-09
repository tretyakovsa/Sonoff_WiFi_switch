/*
 * Sonoff - WiFi switch
 * Arduino IDE 1.6.5
 * esp8266 2.0.0
 * ArduinoJson 5.2.0
 */
#include <ESP8266WiFi.h>        //Содержится в пакете
#include <ESP8266WebServer.h>   //Содержится в пакете
#include <ESP8266SSDP.h>        //Содержится в пакете
#include <FS.h>                 //Содержится в пакете
#include <time.h>               //Содержится в пакете
#include <Ticker.h>             //Содержится в пакете
#include <WiFiUdp.h>            //Содержится в пакете
#include <ESP8266HTTPUpdateServer.h> //Содержится в пакете
#include <ArduinoJson.h>

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
// Определяем переменные
//Обшие настройки
String _ssid     = "WiFi"; // Для хранения SSID
String _password = "Pass"; // Для хранения пароля сети
String _ssidAP = "Sonoff";   // SSID AP точки доступа
String _passwordAP = ""; // пароль точки доступа
String XML;              // формирование XML
String _setAP ="1";           // AP включен
String SSDP_Name = "Sonoff";      // SSDP
String Devices = "";    // IP адреса устройств в сети
int timezone = 3;        // часовой пояс GTM
String kolibrTime = "03:00:00"; // Время колибровки часов
// Для работы с лампой
String times1 = "00:00:00";      // Таймер 1
String times2 = "00:00:00";    // Таймер 2
int TimeLed = 10;  // Время работы Ламп
int ledVolume = 0;

volatile int chaingtime = LOW;
volatile int chaing = LOW;
volatile int  chaingRip = LOW;
int state0 = 0;

unsigned int localPort = 2390;
unsigned int ssdpPort = 1900;

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

void setup() {
 Serial.begin(115200);
 Serial.println("");
   pinMode(rele1, OUTPUT);
 // Параметры памяти ESP справочно можно закаментировать
 CheckFlashConfig();
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
}

void loop() {
 HTTP.handleClient();
 delay(1);
 handleUDP();

 if (chaing) {
    noInterrupts();
    digitalWrite(rele1,!state0);
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
 if (times1.compareTo(Time) == 0 && times1 != "00:00:00") {
 Time01();
 }
 if (times2.compareTo(Time) == 0 && times2 != "00:00:00") {
 Time01();
 }
 if (kolibrTime.compareTo(Time) == 0) {
  chaingtime=1;
 }
}

