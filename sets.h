#include <ESP8266WiFi.h>             //Содержится в пакете
#include <ESP8266WebServer.h>        //Содержится в пакете
#include <ESP8266SSDP.h>             //Содержится в пакете
#include <FS.h>                      //Содержится в пакете
#include <time.h>                    //Содержится в пакете
#include <WiFiUdp.h>                 //Содержится в пакете
#include <ESP8266HTTPUpdateServer.h> //Содержится в пакете
#include <ESP8266httpUpdate.h>       //Содержится в пакете
#include <ESP8266HTTPClient.h>       //Содержится в пакете
#include <DNSServer.h>               //Содержится в пакете
#include <ArduinoJson.h>             //Ставим через менеджер библиотек
#include <PubSubClient.h>           //https://github.com/Imroy/pubsubclient
#include <TickerScheduler.h>         //https://github.com/Toshik/TickerScheduler
#include <StringCommand.h>           //https://github.com/tretyakovsa/ESP8266-StringCommand
//#include <ESP8266LLMNR.h>
//#include <ESP8266NetBIOS.h>
//#include <WebSocketsServer.h>    //https://github.com/Links2004/arduinoWebSockets
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
#include <IRsend.h>
#include <Bounce2.h>                 // https://github.com/thomasfredericks/Bounce2
#include <LivoloTx.h>                //https://github.com/bitlinker/LivoloTx
#ifdef POW
#include "HLW8012.h"                 // https://github.com/xoseperez/hlw8012

// These are the nominal values for the resistors in the circuit
#define CURRENT_RESISTOR                0.001
#define VOLTAGE_RESISTOR_UPSTREAM       ( 5 * 470000 ) // Real: 2280k
#define VOLTAGE_RESISTOR_DOWNSTREAM     ( 1000 ) // Real 1.009k
#endif


#define NUM_BUTTONS 8
boolean but[NUM_BUTTONS];



byte dev[8][8];
boolean secTest = true;
boolean flag = false;
boolean flagT = false;
boolean thenOk;
boolean pins[18];
//String command = "";                 // В эту строку можно написать команду которая будет исполнена в основном цикле
String Lang = "";                    // файлы языка web интерфейса
String chipID = "";
String configJson = "{}";            // Здесь все статусы /config.live.json
String testJson = "{}";
String configOptions = "{}";         // Здесь данные для всех страниц /config.options.json
String configSetup = "{}";           // Здесь данные для setup /config.setup.json
String ssdpList = "{}";              // SSDP имя ip всех модулей в сети /ssdp.list.json
String regCommands = "{\"command\":[]}";           // список доступных комманд для различных модулей /command.json
String previousSetup;                // !!! Предыдущий Setup временная мера будет удалена.
String Scenary;                      // Строка обработки сценариев
String modules = "{\"ip\":\"\",\"SSDP\":\"\",\"space\":\"\",\"module\":[]}";
String sensorsList = "{}";
String prefix   = "/IoTmanager";
String statusS   = "{}";
String minTime;
String comTime;
/*
  ------------------------------------------------------------------------------------------------
  Здесь будут храниться все текстовые константы для сокращения кода
*/
const String fileConfigS   = "config.save.json";
const String ScenaryS   = "scenary.save.txt";
const String configSensorS   = "config.sensor.json";
const String configTimerS   = "timer.save.json";

const String texthtmlS   = "text/html";
const String emptyS   = "";

const String langS   = "lang";
const String setIndexS   = "setIndex";
const String ssdpS   = "SSDP";
const String spaceS   = "space";
const String ssidS   = "ssid";
const String ssidPassS   = "ssidPass";
const String wifiConnectS   = "wifiConnect";
const String wifiBlinkS   = "wifiBlink";
const String checkboxIPS   = "checkboxIP";
const String ipS   = "ip";
const String subnetS   = "subnet";
const String getwayS   = "getway";
const String dnsS   = "dns";
const String timeZoneS   = "timeZone";
const String ssidAPS   = "ssidAP";
const String ssidApPassS   = "ssidApPass";
const String configsS   = "configs";
const String ddnsS   = "ddns";
const String ddnsNameS   = "ddnsName";
const String ddnsPortS   = "ddnsPort";
const String mqttServerS   = "mqttServer";
const String mqttPortS   = "mqttPort";
const String mqttUserS   = "mqttUser";
const String mqttPassS   = "mqttPass";
const String timeS   = "time";
const String macS   = "mac";
const String spiffsDataS   = "spiffsData";
const String buildDataS   = "buildData";
const String spiffsS   = "spiffs";
const String buildS   = "build";

const String ntp1S   = "NTP1";
const String ntp2S   = "NTP2";
const String webSocketS   = "webSocket";
const String weekdayS   = "weekday";
const String relayPinS   = "relayPin";
const String stateRelayS   = "stateRelay";
const String relayNotS   = "relayNot";

const String titleS   = "title";
const String classS   = "class";
const String btnS   = "btn btn-block btn-lg btn-";
const String infoS   = "info";
const String primaryS   = "primary";
const String stateTachS   = "stateTach";
const String buttonNumS   = "buttonNum";

const String irReceivedS   = "irReceived";
const String irDecodeTypeS   = "irDecodeType";
const String irTransmitterS   = "irTransmitter";

const String rfReceivedS   = "rfReceived";
const String rfBitS   = "rfBit";
const String rfProtocolS   = "rfProtocol";

const String stateMovementS   = "stateMovement";
const String movementTimeS   = "movementTime";

const String stateRGBS   = "stateRGB";
const String colorRGBS   = "colorRGB";
const String brightnessRGBS   = "brightnessRGB";
const String modeRGBS   = "modeRGB";
const String speedRGBS   = "speedRGB";
const String timeRGBS   = "timeRGB";
const String timeBUZS   = "timeBUZ";

const String stateSRGBS   = "stateSRGB";
const String colorSRGBS   = "colorSRGB";
const String brightnessSRGBS   = "brightnessSRGB";
const String modeSRGBS   = "modeSRGB";
const String speedSRGBS   = "speedSRGB";

const String relayS   = "relay";

const String turnS   = "turn";
const String pinTurnS   = "pinTurn";
const String pinMotorS   = "pinMotor";
const String stateJalousieS   = "stateJalousie";
const String turnSensorS   = "turnSensor";

const String irsendS   = "irsend";
const String temperatureS   = "temperature";
const String highalarmtempS   = "highalarmtemp";
const String lowalarmtempS   = "lowalarmtemp";
const String alarmtempS   = "alarmtemp";

#ifdef POW
const String ActivePowerWS   = "ActivePowerW";
const String highalarmpowS   = "highalarmpow";
const String lowalarmpowS   = "lowalarmpow";
const String alarmpowS   = "alarmpow";

const String voltagevS   = "voltagev";
const String currentaS   = "currenta";
const String apparentpowervaS   = "apparentpowerva";
const String powerfactorS   = "powerfactor";
const String aggenergywsS   = "aggenergyws";
#endif

const String highalarmA0S   = "highalarmA0";
const String lowalarmA0S   = "lowalarmA0";
const String alarmA0S   = "alarmA0";

const String humidityS   = "humidity";
const String highalarmhumS   = "highalarmhum";
const String lowalarmhumS   = "lowalarmhum";
const String alarmhumS   = "alarmhum";

const String movementS   = "movement";
const String stateA0S   = "stateA0";
const String rgbS   = "rgb";
const String rgbSS   = "rgbs";

const String stateFurnaceS   = "stateFurnace";
const String furnacePin1S   = "furnacePin1-";
const String furnacePin2S   = "furnacePin2-";

const String stateBuzzerS   = "stateBuzzer";
const String buzzerPinS   = "buzzerPin";
const String BuzzerNotS   = "BuzzerNot";

