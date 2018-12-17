// 400 249
//#define Si7021
//#define POW
//#define mqttM //9496
#define ddnsM //1384
#define rgbM // 1824
#define rgbShimM //2480
#define irTransmitterM //9312
#define pinOutM // 1608
#define JalousieM //1408
//#define safeData
//#define macros
//#define webSocket
#include "sets.h"

void setup() {
  //Serial.begin(115200);
  //Serial.println();
  chipID = String( ESP.getChipId() ) + "-" + String( ESP.getFlashChipId() );
  TickerScheduler(1);
  SPIFFS.begin();
  HTTP.begin();
  configSetup = readFile(fileConfigS, 4096 );
  initCMD();
  initWIFI();
  initHTTP();
  //initUpgrade();
  initFS();
  initSSDP();
  initScenary();
  setupToInit();
#ifdef webSocket
  initWebSocket();
#endif
  //testPin();
}

void loop() {
  ts.update();
  HTTP.handleClient();
  dnsServer.processNextRequest();
  handleSSDP();
  handleScenary();
  HTTPWAN.handleClient();
  #ifdef mqttM
  handleMQTT();
  #endif
  ws2812fx.service();
  handleRfReceiv();
  handleButtons();
#ifdef webSocket
  webSocket.loop();
#endif
}
