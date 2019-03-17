// Плагин для загрузки FS https://github.com/esp8266/arduino-esp8266fs-plugin/releases/download/0.3.0/ESP8266FS-0.3.0.zip
#include <ESP8266WiFi.h>             //Содержится в пакете
#include <ESP8266WebServer.h>        //Содержится в пакете
ESP8266WebServer HTTP(80);
ESP8266WebServer HTTPWAN(8080);
#include <ESP8266SSDP.h>             //Содержится в пакете
#include <FS.h>                      //Содержится в пакете
#include <ESP8266HTTPUpdateServer.h> //Содержится в пакете
ESP8266HTTPUpdateServer httpUpdater;
#include <ESP8266httpUpdate.h>       //Содержится в пакете
File fsUploadFile;
#include <time.h>                    //Содержится в пакете
#include <WiFiUdp.h>                 //Содержится в пакете
WiFiUDP udp;
#include <ESP8266HTTPClient.h>       //Содержится в пакете
WiFiClient wclient;
#include <DNSServer.h>               //Содержится в пакете
DNSServer dnsServer;
#include <ArduinoJson.h>             //Ставим через менеджер библиотек
#ifdef mqttM
#include <PubSubClient.h>           //https://github.com/Imroy/pubsubclient
PubSubClient client(wclient);
#endif
#include <TickerScheduler.h>         //https://github.com/Toshik/TickerScheduler
enum { tNTP, tSSDP, tIP, tA0, tDS, tDHT, tSI, tIR, tRC, tPOW, tRSSI, tMQTT, tDDNS, tCRIB };
TickerScheduler ts(15);
#include <StringCommand.h>           //https://github.com/tretyakovsa/ESP8266-StringCommand
StringCommand sCmd;                   // Обьект для работы с командами
//#include <ESP8266LLMNR.h>
//#include <ESP8266NetBIOS.h>
#ifdef webSocketM
#include <WebSocketsServer.h>    //https://github.com/Links2004/arduinoWebSockets
WebSocketsServer webSocket = WebSocketsServer(81);
#endif
// Библиотеки устройств
#include <DHT.h>                     //https://github.com/markruys/arduino-DHT
DHT dht;
#include <OneWire.h>                 //Ставим через менеджер библиотек
#include <DallasTemperature.h>       //Ставим через менеджер библиотек
OneWire *oneWire;
DallasTemperature sensors;
#include <Adafruit_NeoPixel.h>       //https://github.com/adafruit/Adafruit_NeoPixel
#include <WS2812FX.h>                //https://github.com/renat2985/WS2812FX
//WS2812FX ws2812fx = WS2812FX();
WS2812FX ws2812fx = WS2812FX(60, 5, NEO_GRB + NEO_KHZ800);
#include <RCSwitch.h>                //https://github.com/sui77/rc-switch/
#include <IRremoteESP8266.h>         //https://github.com/markszabo/IRremoteESP8266
#include <IRrecv.h>
#include <IRutils.h>
#include <IRsend.h>
RCSwitch mySwitch = RCSwitch();
IRrecv *irReceiver;
decode_results results;
IRsend *irSender;
#include <LivoloTx.h>                //https://github.com/bitlinker/LivoloTx
LivoloTx *gLivolo;
#ifdef Si7021
#include "Adafruit_Si7021.h"         //https://github.com/adafruit/Adafruit_Si7021
Adafruit_Si7021 sensor_Si7021 = Adafruit_Si7021();
#endif
#ifdef POW
#include "HLW8012.h"                 // https://github.com/xoseperez/hlw8012
HLW8012 hlw8012;
// These are the nominal values for the resistors in the circuit
#define CURRENT_RESISTOR                0.001
#define VOLTAGE_RESISTOR_UPSTREAM       ( 5 * 470000 ) // Real: 2280k
#define VOLTAGE_RESISTOR_DOWNSTREAM     ( 1000 ) // Real 1.009k
#endif
#ifdef DimmerM
#include "hw_timer.h"
#endif
#include <Ticker.h>
Ticker flipper[8];


#include <Bounce2.h>                 // https://github.com/thomasfredericks/Bounce2
#define NUM_BUTTONS 8
boolean but[NUM_BUTTONS];
Bounce * buttons = new Bounce[NUM_BUTTONS];

byte fade = 0;
byte tarBrightness = 255;
byte curBrightness = 0;
byte zcState = 0; // 0 = ready; 1 = processing;
String mqttCom;
boolean flag = false;
boolean thenOk;
boolean pins[21];
String ids;
String order;
//String command = emptyS;                 // В эту строку можно написать команду которая будет исполнена в основном цикле
String Lang;                    // файлы языка web интерфейса
String chipID;
String pulsList = "{}";
uint8_t pulsNum =1;
String configJson = "{}";            // Здесь все статусы /config.live.json
String configOptions = "{}";         // Здесь данные для всех страниц /config.options.json
String configSetup = "{}";           // Здесь данные для setup /config.setup.json
String configwidgets;            // Здесь список виджетов для загрузки
String ssdpList = "{}";              // SSDP имя ip всех модулей в сети /ssdp.list.json
String regCommands = "{\"command\":[]}";           // список доступных комманд для различных модулей /command.json
//String previousSetup;                // !!! Предыдущий Setup временная мера будет удалена.
String Scenary;                      // Строка обработки сценариев
String modules = "{\"module\":[]}";
String prefix   = "/IoTmanager";
String statusS   = "{}";
String minTime;
String comTime;
String runTime;
String idTime;
//String pTime;
//String comTimeP;
/*
  ------------------------------------------------------------------------------------------------
  Здесь будут храниться все текстовые константы для сокращения кода
*/
const String fileConfigS   = "config.save.json"; // Имя файла конфигурайии
const String ScenaryS   = "scenary.save.txt";    // Имя файла сценарии
//const String configSensorS   = "config.sensor.json";
const String configTimerS   = "timer.save.json"; // Имя файла таймеры
// общие
const String texthtmlS   = "text/html";
const String emptyS   = emptyS;
const String titleS   = "title";
const String classS   = "class";
const String btnS   = "btn btn-block btn-lg btn-";
const String infoS   = "info";
const String primaryS   = "primary";
const String onS   = "on";
const String offS   = "off";
const String notS   = "not";
const String langOffS = "{{LangOff}}";
const String langOnS = "{{LangOn}}";
const String wifiS   = "wifi";
const String messageS   = "message";
const String voiceS   = "voice";

// конфигурация
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
const String mqttS   = "mqtt";
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
const String weekdayS   = "weekday";
const String webSocketS   = "webSocket";
// пульс
const String pulseS   = "pulse";
const String pulseComS ="pulseCom";
const String pulseTimeS ="pulseTime";
// Реле
const String relayS   = "relay"; //"stateRelay"; // Состояние реле
const String PinS   = "Pin"; // Для хранения ножек реле
const String NotS   = "Not"; // Для хранения признака инверсии
// PinOut
const String pinOutS   = "pinout"; //"stateRelay"; // Состояние реле
#ifdef DimmerM
// Dimmer
const String dimmerS   = "dimmer"; //"DimmerS"; // Состояние димера
  byte stateDim=1;
  byte outPinDim;
#endif


// RGB WS
const String stateRGBS   = "rgb"; //"stateRGB"; Состояние RGB WS
const String colorRGBS   = "colorRGB";
const String brightnessRGBS   = "brightnessRGB";
const String modeRGBS   = "modeRGB";
const String speedRGBS   = "speedRGB";
const String rgbS   = "rgb";

// RGB шим
//const String stateSRGBS   = "rgbs"; // "stateSRGB"; Состояние RGB шим
const String colorSRGBS   = "colorSRGB";
const String brightnessSRGBS   = "brightnessSRGB";
const String modeSRGBS   = "modeSRGB";
const String speedSRGBS   = "speedSRGB";
const String rgbSS   = "rgbs"; //Состояние RGB шим

// Жалюзи
#ifdef JalousieM
const String stateJalousieS   = "jalousie";
const String turnS   = "turn";
const String pinTurnS   = "pinTurn";
const String pinMotorS   = "pinMotor";
const String turnSensorS   = "turnSensor";
const String LangOpenS   = "{{LangOpen}}";
const String LangCloseS   = "{{LangClose}}";
#endif
// Звуковой сигнал
//const String stateBuzzerS   = "stateBuzzer";
const String buzzerPinS   = "buzzerPin";
const String toneS   = "tone";

// Кнопки
const String stateTachS   = "Tach"; // Состояние кнопки
const String tachS   = "tach";
const String invTachS   = "invTach";

// ИК передатчик приемник
const String irReceivedS   = "irReceived";
const String irDecodeTypeS   = "irDecodeType";
const String irTransmitterS   = "irTransmitter";
const String rfReceivedS   = "rfReceived";
const String rfBitS   = "rfBit";
const String rfProtocolS   = "rfProtocol";
const String irsendS   = "irsend";
// RF передатчик
const String rfsendS   = "rfsend";
// RF передатчик Livolo
const String lvsendS   = "lvsend";
// Температура
const String temperatureS   = "temperature";
const String highalarmtempS   = "highAlarmTemp";
const String lowalarmtempS   = "lowAlarmTemp";
const String alarmtempS   = "alarmTemp";

// Влажность
const String humidityS   = "humidity";
const String highalarmhumS   = "highAlarmHum";
const String lowalarmhumS   = "lowAlarmHum";
const String alarmhumS   = "alarmHum";

// Ocвещенность
const String stateA0S   = "A0";
const String highalarmA0S   = "highAlarmA0";
const String lowalarmA0S   = "lowAlarmA0";
const String alarmA0S   = "alarmA0";

// Датчик мощьности
#ifdef POW
const String ActivePowerWS   = "ActivePowerW";
const String highalarmpowS   = "highAlarmPow";
const String lowalarmpowS   = "lowAlarmPow";
const String alarmpowS   = "alarmPow";
const String voltagevS   = "voltagev";
const String currentaS   = "currenta";
const String apparentpowervaS   = "apparentpowerva";
const String powerfactorS   = "powerfactor";
const String aggenergywsS   = "aggenergyws";
#endif
