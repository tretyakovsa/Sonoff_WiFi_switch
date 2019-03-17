// 392997 - 446904 = 44655
//#define Si7021
//#define POW
//#define mqttM //9496
#define ddnsM //1384 - 1012
#define rgbM // 1824 - 2700
#define rgbShimM //2480 - 2016
//#define irTransmitterM //9312
#define pinOutM // 1608 - 1848
//#define JalousieM //1408 - 1628
//#define DimmerM // 784
//#define safeData
//#define macros
//#define webSocketM
#include "sets.h"

void setup() {
  Serial.begin(115200);
  Serial.println();
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
#ifdef webSocketM

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
  #ifdef rgbM
  ws2812fx.service();
  //ws2812fx[1].service();
  #endif
  handleRfReceiv();
  handleButtons();
#ifdef webSocketM
  webSocket.loop();
#endif
}
