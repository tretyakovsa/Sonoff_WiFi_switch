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
  Serial.begin(115200);
  delay(100);
  TickerScheduler(1);
  Serial.println ("");
  Serial.println ("Load");
  initCMD();
  chipID = String( ESP.getChipId() ) + "-" + String( ESP.getFlashChipId() );
  FS_init();         // Включаем работу с файловой системой
  configJson = readFile("config.save.json", 1024);
  String configs = jsonRead(configJson, "configs");
  configs.toLowerCase();

  String test = readFile("configs/"+configs+".txt", 4096);
  //Serial.print(test);
  test.replace("\r\n", "\n");
   test +="\n";
  //Serial.print(test);
   sCmd.readStr("wifi 12");
   sCmd.readStr("Upgrade");
   sCmd.readStr("SSDP");
   sCmd.readStr("HTTP");

  Serial.println(goCommands(test));
  //Serial.println(modules);
  //Serial.println(modules.lastIndexOf("[]"));
  /*
  if(modules.lastIndexOf("[]")!=-1){
    String rn = "\n";
    //test ="Serial 115200"+rn;
    test ="wifi 12"+rn;
    test +="Upgrade"+rn;
    test +="SSDP"+rn;
    test +="HTTP"+rn;
    Serial.println(goCommands(test));
    }
   */
  Serial.println (configLive);
  Serial.println ("Start");
  configJson = jsonWrite(configJson, "mac", WiFi.macAddress().c_str());
}

void loop() {
  ts.update();
  sCmd.readStr(command);     // We don't do much, just process serial commands
  command = "";
  dnsServer.processNextRequest();
  HTTPWAN.handleClient();
  delay(1);
  HTTP.handleClient();
  delay(1);
  handleUDP();
  handleMQTT();
  ws2812fx.service();
}
