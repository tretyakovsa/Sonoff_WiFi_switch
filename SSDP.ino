void initSSDP() {
  // Включаем определение имени для Windows
  // Модуль будет доступен по запросу вида
  String temp = jsonRead(configJson, "SSDP");
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
  SSDP.setName(jsonRead(configJson, "SSDP"));
  SSDP.setSerialNumber(chipID);
  SSDP.setURL("/index.htm");
  SSDP.setModelName(jsonRead(configJson, "configs"));
  SSDP.setModelNumber(chipID);
  SSDP.setModelURL("https://github.com/tretyakovsa/Sonoff_WiFi_switch");
  SSDP.setManufacturer("Tretyakov Sergey, Kevrels Renats");
  SSDP.setManufacturerURL("http://www.esp8266-arduinoide.ru");
  SSDP.begin();

  // Установить имя устройства
  HTTP.on("/device", handle_device);        // Установить имя устройства
  HTTP.on("/ip.list.json", HTTP_GET, []() {
    HTTP.send(200, "text/plain", addressList);
  });
}





// ------------- Установить имя устройства
void handle_device() {
  // /device?ssdp=Sonoff-Rele&space={{space}}
  String  ssdpName = HTTP.arg("ssdp");
  configJson = jsonWrite(configJson, "SSDP", ssdpName);
  configJson = jsonWrite(configJson, "space", HTTP.arg("space"));
  SSDP.setName(ssdpName);
  HTTP.send(200, "text/plain", "Ok");
  writeFile("config.save.json", configJson );
}

// ------------- SSDP запрос
void requestSSDP () {
  if (WiFi.status() == WL_CONNECTED) {
    addressList = "{\"ssdpList\":[]}";
    ssdpList(chipID,  WiFi.localIP().toString());
    IPAddress ssdpAdress(239, 255, 255, 250);
    unsigned int ssdpPort = 1900;
    char  ReplyBuffer[] = "M-SEARCH * HTTP/1.1\r\nHost:239.255.255.250:1900\r\nST:upnp:rootdevice\r\nMan:\"ssdp:discover\"\r\nMX:3\r\n\r\n";
    udp.beginPacket(ssdpAdress, ssdpPort);
    udp.write(ReplyBuffer);
    udp.endPacket();
    //Serial.println(addressList);
  }
}
// ------------- Чтение ответа от устройств SSDP слушаем порт все время
void handleUDP() {
  String input_string = "";
  String chipIDremote = "";
  char packetBuffer[512];
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(packetBuffer, 512);
    if (len > 0) packetBuffer[len] = 0;
    input_string += packetBuffer;
    //Serial.println(input_string);
    int i = input_string.indexOf("Arduino");
    if (i > 0) {
      chipIDremote = deleteBeforeDelimiter(input_string, "Arduino");
      chipIDremote = selectToMarker(chipIDremote, "\n");
      chipIDremote = selectToMarkerLast(chipIDremote, "/");
      chipIDremote = selectToMarker(chipIDremote, "\r");
      // строку input_string сохраняю для расширения
      ssdpList(chipIDremote, udp.remoteIP().toString());
    }
    //Serial.println(addressList);
  }
}
void ssdpList(String chipIDremote, String remoteIP ) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& list = jsonBuffer.parseObject(addressList);
  JsonArray& arrays = list["ssdpList"].asArray();
  JsonObject& record = arrays.createNestedObject();
  record["id"]  = chipIDremote;
  record["ip"]  = remoteIP;
  addressList = "";
  list.printTo(addressList);
  //list.printTo(Serial);
}

// Каждые 30 секунд проверяем не изиенился ли адрес ip
void ipChanges() {
  String ip = WiFi.localIP().toString();
  if (jsonRead(configJson, "ip") != ip) {
    configJson = jsonWrite(configJson, "ip", ip);
    requestSSDP();

  }
}

// {"ssdpList":[{"id":"14317906-1458400","ip":"192.168.0.102"},{"id":"16321347-1327328","ip":"192.168.0.108"}]}

void deviceList() {
  HTTP.send(200, "text/plain", "[" + modules + "]");
}




