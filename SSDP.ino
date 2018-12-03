void initSSDP() {
  String temp = getSetup(ssdpS);
  if (temp == emptyS or temp == "Sonoff") {
    temp = "Sonoff";//+jsonRead(configSetup, macS);
    sendSetup(ssdpS, temp);
    //sendStatus(ssdpS, temp);
  }
   HTTP.on("/ssdp.list.json", HTTP_GET, []() {
      httpOkJson(ssdpList);
    });
  if (getStatusInt(wifiS) == 3) { //Если есть подключение к роутеру
    // Включаем определение имени для Windows
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
    SSDP.setName(getSetup(ssdpS));
    SSDP.setSerialNumber(chipID);
    SSDP.setURL("/");
    SSDP.setModelName(getSetup(configsS));
    SSDP.setModelNumber(chipID + "/" + getSetup(ssdpS));
    SSDP.setModelURL("https://github.com/tretyakovsa/Sonoff_WiFi_switch");
    SSDP.setManufacturer("Tretyakov Sergey, Kevrels Renats");
    SSDP.setManufacturerURL("http://www.esp8266-arduinoide.ru");
    SSDP.begin();

    // Установить имя устройства
    HTTP.on("/device", HTTP_GET, []() {
      String  ssdpName = HTTP.arg("ssdp");
      sendSetup(ssdpS, ssdpName);
      sendOptions(ssdpS, ssdpName);
      SSDP.setName(ssdpName);
      SSDP.setModelNumber(chipID + "/" + getSetup(ssdpS));
      String  space = HTTP.arg("space");
      sendSetup(spaceS, space);
      sendOptions(spaceS, space);
      jsonWrite(modules, ssdpS, getSetup(ssdpS));
      jsonWrite(modules, spaceS, space);
      httpOkText();
      saveConfigSetup();
      requestSSDP();
    });

    // Каждые 60 секунд проверяем не изиенился ли адрес ip
    ts.add(tIP, 60000, [&](void*) {
      ipChanges();
      //    Serial.println("SSDP");
      //safeDataToFile(1, stateA0S, getStatusInt(stateA0S));
    }, nullptr, true);
    // задача проверять наличие устройств в сети каждые 60 секунд.
    ts.add(tSSDP, 60000, [&](void*) {
      requestSSDP();
      //Serial.println(ssdpList);
      //Serial.println("requestSSDP "+GetTime());
    }, nullptr, true);
  }
}
// ------------- SSDP запрос
void requestSSDP () {
  if (WiFi.status() == WL_CONNECTED) {
    ssdpList="{}";
    jsonWrite(ssdpList, getSetup(ssdpS), WiFi.localIP().toString());
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
  if (getStatusInt(wifiS) == 3) { //Если есть подключение к роутеру
    String input_string;
    String chipIDremote;
    String ssdpName;
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
}
// Каждые 30 секунд проверяем не изиенился ли адрес ip
void ipChanges() {
  String ip = WiFi.localIP().toString();
  if (jsonRead(configSetup, ipS) != ip) {
    sendStatus(ipS, ip);
    jsonWrite(configSetup, ipS, ip);
    jsonWrite(modules, ipS, getOptions(ipS));
  }
}
