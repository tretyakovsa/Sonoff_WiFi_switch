//#define Si7021
//#define POW
//#define CRIB
#include "sets.h"             //Содержится в пакете


DNSServer dnsServer;
ESP8266WebServer HTTP(80);
File fsUploadFile;
TickerScheduler ts(15);
WiFiUDP udp;
StringCommand sCmd;                   // Обьект для работы с командами
ESP8266HTTPUpdateServer httpUpdater;
WiFiClient wclient;
PubSubClient client(wclient);
//WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer HTTPWAN(8080);

RCSwitch mySwitch = RCSwitch();
IRrecv *irReceiver;
decode_results results;
IRsend *irSender;
LivoloTx *gLivolo;
WS2812FX ws2812fx = WS2812FX();
#ifdef CRIB
Ticker flipper;
#endif
#ifdef POW
HLW8012 hlw8012;
#endif

OneWire *oneWire;
DallasTemperature sensors;
DHT dht;

#ifdef Si7021
Adafruit_Si7021 sensor_Si7021 = Adafruit_Si7021();
#endif

Bounce * buttons = new Bounce[NUM_BUTTONS];

void setup() {
Serial.begin(115200);
Serial.println();
  start_init();
}

void loop() {
  ts.update();
  HTTP.handleClient();
  dnsServer.processNextRequest();
  handleSSDP();
  handleScenary();
  HTTPWAN.handleClient();
  handleMQTT();
  ws2812fx.service();
  handleRfReceiv();
  handleButtons();
  // webSocket.loop();
}
