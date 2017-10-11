void initSSDP() {
  // Включаем определение имени для Windows
  // Модуль будет доступен по запросу вида
  String temp = jsonRead(configSetup, "SSDP");
  if (temp == "") {
    temp = "Sonoff";
    configSetup = jsonWrite(configSetup, "SSDP", temp);
    saveConfigSetup ();
  }
  ssdpList = jsonWrite(ssdpList, jsonRead(configSetup, "SSDP"), WiFi.localIP().toString());
  //LLMNR.begin(temp.c_str());
  //NBNS.begin(temp.c_str());
  unsigned int localPort = 1901;
  udp.begin(localPort);
  // задача проверять смену ip каждые 30 секунд
  ts.add(1, 30000, [&](void*) {
    ipChanges();
  }, nullptr, true);
  // задача проверять наличие устройств в сети каждые две минуты.
  ts.add(2, 120000, [&](void*) {
    requestSSDP();
  }, nullptr, true);
  // SSDP дескриптор
  HTTP.on("/description.xml", HTTP_GET, []() {
    SSDP.schema(HTTP.client());
  });
  SSDP.setDeviceType("upnp:rootdevice");
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort(80);
  SSDP.setName(jsonRead(configSetup, "SSDP"));
  SSDP.setSerialNumber(chipID);
  SSDP.setURL("/");
  SSDP.setModelName(jsonRead(configSetup, "configs"));
  SSDP.setModelNumber(chipID + "/" + jsonRead(configSetup, "SSDP"));
  SSDP.setModelURL("https://github.com/tretyakovsa/Sonoff_WiFi_switch");
  SSDP.setManufacturer("Tretyakov Sergey, Kevrels Renats");
  SSDP.setManufacturerURL("http://www.esp8266-arduinoide.ru");
  SSDP.begin();

  // Установить имя устройства
  HTTP.on("/device", handle_device);        // Установить имя устройства
  HTTP.on("/ip.list.json", HTTP_GET, []() {
    HTTP.send(200, "application/json", addressList);
  });
    HTTP.on("/ssdp.list.json", HTTP_GET, []() {
    HTTP.send(200, "application/json", ssdpList);
  });
}

// ------------- Установить имя устройства
void handle_device() {
  // /device?ssdp=Sonoff-Rele&space={{space}}
  String  ssdpName = HTTP.arg("ssdp");
  configSetup = jsonWrite(configSetup, "SSDP", ssdpName);
  configOptions = jsonWrite(configOptions, "SSDP", ssdpName);
  modules = jsonWrite(modules, "SSDP", ssdpName);
  SSDP.setName(ssdpName);
  SSDP.setModelNumber(chipID + "/" + jsonRead(configSetup, "SSDP"));
  String  space = HTTP.arg("space");
  configSetup = jsonWrite(configSetup, "space", space);
  configOptions = jsonWrite(configOptions, "space", space);
  modules = jsonWrite(modules, "space", space);
  HTTP.send(200, "text/plain", "Ok");
  saveConfigSetup();
}

// ------------- SSDP запрос
void requestSSDP () {
  if (WiFi.status() == WL_CONNECTED) {
    //addressList = "{\"ssdpList\":[]}";
    //ssdpLists(chipID,  WiFi.localIP().toString(), jsonRead(configSetup, "SSDP"));
    ssdpList = jsonWrite(ssdpList, jsonRead(configSetup, "SSDP"), WiFi.localIP().toString());
    IPAddress ssdpAdress(239, 255, 255, 250);
    unsigned int ssdpPort = 1900;
    char  ReplyBuffer[] = "M-SEARCH * HTTP/1.1\r\nHost:239.255.255.250:1900\r\nST:upnp:rootdevice\r\nMan:\"ssdp:discover\"\r\nMX:3\r\n\r\n";
    udp.beginPacket(ssdpAdress, ssdpPort);
    udp.write(ReplyBuffer);
    udp.endPacket();
  }
}
// ------------- Чтение ответа от устройств SSDP слушаем порт все время
void handleUDP() {
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
      //1.0 UPNP/1.1 smart-room/4039-1458400/IoT-Room 2 (\n)
      ssdpName = selectToMarkerLast(chipIDremote, "/");
      ssdpName = selectToMarker(ssdpName, "\r");
      chipIDremote = selectToMarkerLast(chipIDremote, "/");
      chipIDremote = selectToMarker(chipIDremote, "\r");
      // строку input_string сохраняю для расширения
      ssdpList = jsonWrite(ssdpList, chipIDremote, udp.remoteIP().toString());

    }
  }
}
void ssdpLists(String chipIDremote, String remoteIP, String ssdpName ) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& list = jsonBuffer.parseObject(addressList);
  JsonArray& arrays = list["ssdpList"].asArray();
  JsonObject& record = arrays.createNestedObject();
  //record["id"]  = chipIDremote;
  record["ip"]  = remoteIP;
  record["ssdp"]  = ssdpName;
  addressList = "";
  list.printTo(addressList);
}

// Каждые 30 секунд проверяем не изиенился ли адрес ip
void ipChanges() {
  String ip = WiFi.localIP().toString();
  if (jsonRead(configSetup, "ip") != ip) {
    configJson = jsonWrite(configJson, "ip", ip);
    configSetup = jsonWrite(configSetup, "ip", ip);
    requestSSDP();

  }
}

// {"ssdpList":[{"id":"14317906-1458400","ip":"192.168.0.102"},{"id":"16321347-1327328","ip":"192.168.0.108"}]}

void deviceList() {
  HTTP.send(200, "text/plain", "[" + modules + "]");
}




