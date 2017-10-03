#include <ESP8266WiFi.h>             //Содержится в пакете
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>  // https://github.com/me-no-dev/ESPAsyncWebServer
#include <SPIFFSEditor.h>
#include <ESP8266SSDP.h>             //Содержится в пакете
#include <FS.h>                      //Содержится в пакете
#include <time.h>                    //Содержится в пакете
//#include <Ticker.h>                  //Содержится в пакете
#include <WiFiUdp.h>                 //Содержится в пакете
#include <ESP8266HTTPUpdateServer.h> //Содержится в пакете
#include <ESP8266httpUpdate.h>       //Содержится в пакете
#include <ESP8266HTTPClient.h>       //Содержится в пакете
#include <DNSServer.h>               //Содержится в пакете
#include <ArduinoJson.h>             //Ставим через менеджер библиотек
#include <PubSubClient.h>           //https://github.com/Imroy/pubsubclient
//#include <ESP8266LLMNR.h>
//#include <ESP8266NetBIOS.h>
#include <TickerScheduler.h>         //https://github.com/Toshik/TickerScheduler
#include <StringCommand.h>           //https://github.com/tretyakovsa/ESP8266-StringCommand
// Библиотеки устройств
#include <DHT.h>                     //https://github.com/markruys/arduino-DHT
#include <OneWire.h>                 //Ставим через менеджер библиотек
#include <DallasTemperature.h>       //Ставим через менеджер библиотек
#include <Adafruit_NeoPixel.h>       //https://github.com/adafruit/Adafruit_NeoPixel
#include <WS2812FX.h>                //https://github.com/MTJoker/WS2812FX
#include <RCSwitch.h>                //Ставим через менеджер библиотек


#define d18b20PIN 14
DHT dht;
RCSwitch mySwitch = RCSwitch();

// DNSServer для режима AP
DNSServer dnsServer;

// Web интерфейсы для устройства
//ESP8266WebServer HTTP(80);
AsyncWebServer HTTP(80);
//AsyncWebServer HTTPWAN();
//ESP8266WebServer HTTPWAN;
// Обнавление прошивки
ESP8266HTTPUpdateServer httpUpdater;


// Для файловой системы
File fsUploadFile;

// Для тикера
Ticker motion;

// Для поиска других устройств по протоколу SSDP
WiFiUDP udp;
WiFiClient wclient;
PubSubClient client(wclient);
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(d18b20PIN);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature d18b20(&oneWire);

WS2812FX ws2812fx = WS2812FX();


TickerScheduler ts(5);

boolean secTest = true;
StringCommand sCmd;     // The demo StringCommand object
String command = "";

String Lang = "";                    // файлы языка web интерфейса
String chipID = "";
String configJson = "{}";            // Здесь все статусы
String configOptions = "{}";         // Здесь опции для всех страниц
String configSetup = "{}";           // Здесь данные для setup
String configLive = "{}";            // Здесь внутренние данные обмен пинами и тд
String ssdpList = "{}";
String regCommands = "{}";
String jsonTimer = "{}";
String Scenary;
String Timerset = "";
String modules = "{\"ip\":\"\",\"SSDP\":\"\",\"space\":\"\",\"module\":[]}";
String addressList = "{\"ssdpList\":[]}";
String sensorsList = "{}";
String prefix   = "/IoTmanager";
boolean flag = false;
boolean thenOk;


void setup() {

  //Serial.println (ESP.getResetReason());
  Serial.begin(115200);
  delay(100);
  TickerScheduler(1);
  Serial.println ("");
  Serial.println ("Load");
  initCMD();
  chipID = String( ESP.getChipId() ) + "-" + String( ESP.getFlashChipId() );
  SPIFFS.begin();        // Включаем работу с файловой системой
  // ----------------- начинаем загрузку
  configSetup = readFile("config.save.json", 4096);
  configSetup = jsonWrite(configSetup, "time", "00:00:00");
  //configJson = jsonWrite(configJson, "setIndex", jsonRead(configSetup, "setIndex"));
  configOptions = jsonWrite(configOptions, "lang", jsonRead(configSetup, "lang"));
  configOptions = jsonWrite(configOptions, "SSDP", jsonRead(configSetup, "SSDP"));
  configOptions = jsonWrite(configOptions, "space", jsonRead(configSetup, "space"));
  String configs = jsonRead(configSetup, "configs");
  configs.toLowerCase();
  // ----------- Грузим конфигурацию устройства
  String test = readFile("configs/"+configs+".txt", 4096);
  test.replace("\r\n", "\n");
   test +="\n";
   // ----------- запускаем необходимые всегда модули
   sCmd.readStr("wifi 12");
   sCmd.readStr("Upgrade");
   sCmd.readStr("SSDP");
   sCmd.readStr("HTTP");
   // ----------- Выполняем запуск кофигурации
  Serial.println(goCommands(test));
  test ="";
  configSetup = jsonWrite(configSetup, "mac", WiFi.macAddress().c_str());
  configSetup = jsonWrite(configSetup, "ip", WiFi.localIP().toString());
  initScenary();
}

void loop() {
  ts.update();
  sCmd.readStr(command);     // We don't do much, just process serial commands
  command = "";
  dnsServer.processNextRequest();
  yield();
  handleUDP();
  handleMQTT();
  handleScenary();
}
