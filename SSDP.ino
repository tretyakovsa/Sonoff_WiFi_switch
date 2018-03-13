void initSSDP() {
  // Включаем определение имени для Windows
  // Модуль будет доступен по запросу вида
  String temp = jsonRead(configSetup, ssdpS);
  if (temp == "" or temp == "Sonoff") {
    temp = "Sonoff";//+jsonRead(configSetup, macS);
    jsonWrite(configSetup, ssdpS, temp);
  }
  //LLMNR.begin(temp.c_str());
  //NBNS.begin(temp.c_str());
  unsigned int localPort = 1901;
  udp.begin(localPort);
  // SSDP дескриптор
  HTTP.on("/description.xml", HTTP_GET, []() {
    SSDP.schema(HTTP.client());
  });
  SSDP.setDeviceType("upnp:rootdevice");
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort(80);
  SSDP.setName(jsonRead(configSetup, ssdpS));
  SSDP.setSerialNumber(chipID);
  SSDP.setURL("/");
  SSDP.setModelName(jsonRead(configSetup, configsS));
  SSDP.setModelNumber(chipID + "/" + jsonRead(configSetup, ssdpS));
  SSDP.setModelURL("https://github.com/tretyakovsa/Sonoff_WiFi_switch");
  SSDP.setManufacturer("Tretyakov Sergey, Kevrels Renats");
  SSDP.setManufacturerURL("http://www.esp8266-arduinoide.ru");
  SSDP.begin();

  // Установить имя устройства
  HTTP.on("/device", HTTP_GET, []() {
    String  ssdpName = HTTP.arg("ssdp");
    jsonWrite(configSetup, ssdpS, ssdpName);
    sendOptions(ssdpS, ssdpName);
    SSDP.setName(ssdpName);
    SSDP.setModelNumber(chipID + "/" + jsonRead(configSetup, ssdpS));
    String  space = HTTP.arg("space");
    jsonWrite(configSetup, spaceS, space);
    sendOptions(spaceS, space);
    httpOkText();
    saveConfigSetup();
    requestSSDP();
  });
  HTTP.on("/ssdp.list.json", HTTP_GET, []() {
    httpOkJson(ssdpList);
  });
  // задача проверять наличие устройств в сети каждые три минуты.
  ts.add(1, 180000, [&](void*) {
    ipChanges();
  }, nullptr, true);
  // задача проверять наличие устройств в сети каждые две минуты.
  ts.add(2, 120000, [&](void*) {
        requestSSDP();
        //Serial.println(ssdpList);
        //Serial.println("requestSSDP "+GetTime());
  }, nullptr, true);

}
// ------------- SSDP запрос
void requestSSDP () {
  if (WiFi.status() == WL_CONNECTED) {
    jsonWrite(ssdpList, jsonRead(configSetup, ssdpS), WiFi.localIP().toString());
    IPAddress ssdpAdress(239, 255, 255, 250);
    unsigned int ssdpPort = 1900;
    char  ReplyBuffer[] = "M-SEARCH * HTTP/1.1\r\nHost:239.255.255.250:1900\r\nST:upnp:rootdevice\r\nMan:\"ssdp:discover\"\r\nMX:3\r\n\r\n";
    udp.beginPacket(ssdpAdress, ssdpPort);
    udp.write(ReplyBuffer);
    udp.endPacket();
  }
}
// ------------- Чтение ответа от устройств SSDP слушаем порт все время
void handleSSDP() {
  String input_string = "";
  String chipIDremote = "";
  String ssdpName = "";
  char packetBuffer[512];
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(packetBuffer, 512);
    if (len > 0) packetBuffer[len] = 0;
    input_string += packetBuffer;
        int i = input_string.indexOf("Arduino");
    if (i > 0) {
      chipIDremote = deleteBeforeDelimiter(input_string, "Arduino");
      chipIDremote = selectToMarker(chipIDremote, "\n");
      ssdpName = selectToMarkerLast(chipIDremote, "/");
      ssdpName = selectToMarker(ssdpName, "\r");
      chipIDremote = selectToMarkerLast(chipIDremote, "/");
      chipIDremote = selectToMarker(chipIDremote, "\r");
       // строку input_string сохраняю для расширения
      jsonWrite(ssdpList, chipIDremote, udp.remoteIP().toString());
    }
  }
}
// Каждые 30 секунд проверяем не изиенился ли адрес ip
void ipChanges() {
  String ip = WiFi.localIP().toString();
  if (jsonRead(configSetup, ipS) != ip) {
    sendStatus(ipS, ip);
    jsonWrite(configSetup, ipS, ip);
  }
}







