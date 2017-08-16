#include <ESP8266WiFi.h>             //Содержится в пакете
#include <ESP8266WebServer.h>        //Содержится в пакете
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
ESP8266WebServer HTTP(80);

// Обнавление прошивки
ESP8266HTTPUpdateServer httpUpdater;
ESP8266WebServer HTTPWAN;

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
String configJson = "";
String configLive = "{}";
String jsonTimer = "{}";
String Timerset = "";
String modules = "{\"ip\":\"\",\"SSDP\":\"\",\"space\":\"\",\"module\":[]}";
String addressList = "{\"ssdpList\":[]}";
String sensorsList = "{}";
String prefix   = "/IoTmanager";
//boolean ddnsTest = true;


void setup() {

  //Serial.println (ESP.getResetReason());

  TickerScheduler(1);
  Serial.println ("Load");
  initCMD();
  chipID = String( ESP.getChipId() ) + "-" + String( ESP.getFlashChipId() );
  FS_init();         // Включаем работу с файловой системой
  configJson = readFile("config.save.json", 1024);
  String init = readFile("config.modules.json", 4096);
  String configs = jsonRead(configJson, "configs");

  if (configs == "") {
    sCmd.readStr("Serial 115200");
    sCmd.readStr("wifi 12");
    sCmd.readStr("Upgrade");
    sCmd.readStr("HTTP");
    //configs = "Basic";
  }

  Serial.println(modulesInit(init, configs));
  Serial.println (configLive);
  Serial.println ("Start");
}

void loop() {
  ts.update();
  sCmd.readStr(command);     // We don't do much, just process serial commands
  command = "";
  dnsServer.processNextRequest();
  HTTP.handleClient();
  delay(1);
  handleUDP();
  handleMQTT();
  ws2812fx.service();
}
