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
#include <WS2812FX.h>                //https://github.com/renat2985/WS2812FX
#include <RCSwitch.h>                //https://github.com/sui77/rc-switch/
#include <IRremoteESP8266.h>         //https://github.com/markszabo/IRremoteESP8266
#include <IRrecv.h>
#include <IRutils.h>

#define d18b20PIN 14
DHT dht;
RCSwitch mySwitch = RCSwitch();
IRrecv *irReceiver; 
decode_results results;
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


TickerScheduler ts(6);

boolean secTest = true;
StringCommand sCmd;     // The demo StringCommand object
String command = "";

String Lang = "";                    // файлы языка web интерфейса
String chipID = "";
String configJson = "{}";            // Здесь все статусы
String configOptions = "{}";         // Здесь опции для всех страниц
String configSetup = "{}";           // Здесь данные для setup
String ssdpList = "{}";
String regCommands = "{}";
String jsonTimer = "{}";
String previousSetup;
String Scenary;
String Timerset = "";
String modules = "{\"ip\":\"\",\"SSDP\":\"\",\"space\":\"\",\"module\":[]}";
String addressList = "{\"ssdpList\":[]}";
String sensorsList = "{}";
String prefix   = "/IoTmanager";
boolean flag = false;
boolean thenOk;


void setup() {

  //Serial.begin(115200);
  //delay(100);
  TickerScheduler(1);
  initCMD();
  chipID = String( ESP.getChipId() ) + "-" + String( ESP.getFlashChipId() );
  FS_init();         // Включаем работу с файловой системой
  // ----------------- начинаем загрузку
  configSetup = readFile("config.save.json", 4096);

 //previousSetup = configSetup;
  //configSetup ="{}";
  //Serial.println(configSetup);
//savePrevious();
  


  
  configSetup = jsonWrite(configSetup, "time", "00:00:00");
  //configJson = jsonWrite(configJson, "setIndex", jsonRead(configSetup, "setIndex"));
  configOptions = jsonWrite(configOptions, "lang", jsonRead(configSetup, "lang"));
  configOptions = jsonWrite(configOptions, "SSDP", jsonRead(configSetup, "SSDP"));
  configOptions = jsonWrite(configOptions, "space", jsonRead(configSetup, "space"));
  configOptions = jsonWrite(configOptions, "spiffsData", jsonRead(configSetup, "spiffsData"));
  configOptions = jsonWrite(configOptions, "buildData", jsonRead(configSetup, "buildData"));
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
goCommands(test);
  test = "";
  configSetup = jsonWrite(configSetup, "mac", WiFi.macAddress().c_str());
  configSetup = jsonWrite(configSetup, "ip", WiFi.localIP().toString());

  initScenary();
}

void loop() {
  ts.update();
  sCmd.readStr(command);     
  command = "";
  dnsServer.processNextRequest();
  HTTPWAN.handleClient();
  yield();
  HTTP.handleClient();
  yield();
  handleUDP();
  handleMQTT();
  ws2812fx.service();
  handleScenary();
  //RCRCreceiv();
}
