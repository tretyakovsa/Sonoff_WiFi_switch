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
  // ---------------------- SSDP дескриптор
  HTTP.on("/device", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasArg("ssdp")) {
      String arg = request->arg("ssdp");
      configSetup = jsonWrite(configSetup, "SSDP", arg);
      configLive = jsonWrite(configLive, "SSDP", arg);
    }
    request->send(200, "text/json", "Ok");
    saveConfigSetup();
  });

  HTTP.on("/description.xml", HTTP_GET, [](AsyncWebServerRequest * request) {
    String ssdpSend = "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">";
    String  ssdpHeder = xmlNode("major", "1");
    ssdpHeder += xmlNode("minor", "0");
    ssdpHeder = xmlNode("specVersion", ssdpHeder);
    ssdpHeder += xmlNode("URLBase", "http://" + WiFi.localIP().toString());
    String  ssdpDescription = xmlNode("deviceType", "upnp:rootdevice");
    ssdpDescription += xmlNode("friendlyName", jsonRead(configSetup, "SSDP"));
    ssdpDescription += xmlNode("presentationURL", "/");
    ssdpDescription += xmlNode("serialNumber", chipID);
    ssdpDescription += xmlNode("modelName", jsonRead(configSetup, "configs"));
    ssdpDescription += xmlNode("modelNumber", chipID + "/" + jsonRead(configSetup, "SSDP"));
    ssdpDescription += xmlNode("modelURL", "https://github.com/tretyakovsa/Sonoff_WiFi_switch");
    ssdpDescription += xmlNode("manufacturer", "Tretyakov Sergey, Kevrels Renats");
    ssdpDescription += xmlNode("manufacturerURL", "http://www.esp8266-arduinoide.ru");
    ssdpDescription += xmlNode("UDN", "uuid:38323636-4558-4dda-9188-cda0e6" + decToHex(ESP.getChipId(), 6));
    ssdpDescription = xmlNode("device", ssdpDescription);
    ssdpHeder += ssdpDescription;
    ssdpSend += ssdpHeder;
    ssdpSend += "</root>";
    request->send(200, "text/xml", ssdpSend);

  });
  //Если версия  2.0.0 закаментируйте следующую строчку
  SSDP.setDeviceType("upnp:rootdevice");
  SSDP.setSchemaURL("description.xml");
  SSDP.setName(jsonRead(configSetup, "SSDP"));
  //SSDP.setSerialNumber(chipID);
  //SSDP.setURL("/");
  SSDP.setModelName(jsonRead(configSetup, "configs"));
  SSDP.setModelNumber(chipID + "/" + jsonRead(configSetup, "SSDP"));
  SSDP.begin();

  // Установить имя устройства
 // HTTP.on("/device", handle_device);        // Установить имя устройства

  // --------------------Выдаем данные ssdpList
  HTTP.on("/ip.list.json", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "application/json", addressList);
  });
  // --------------------Выдаем данные addressList
  HTTP.on("/ssdp.list.json", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "application/json", ssdpList);
  });
}

// ------------- SSDP запрос
void requestSSDP () {
  if (WiFi.status() == WL_CONNECTED) {
    addressList = "{\"ssdpList\":[]}";
    ssdpLists(chipID,  WiFi.localIP().toString(), jsonRead(configSetup, "SSDP"));
    ssdpList = jsonWrite(ssdpList, jsonRead(configSetup, "SSDP"), WiFi.localIP().toString());
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
  String ssdpName = "";
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
      //Serial.println(chipIDremote);
      chipIDremote = selectToMarker(chipIDremote, "\n");
      //Serial.println(chipIDremote);
      //1.0 UPNP/1.1 smart-room/4039-1458400/IoT-Room 2 (\n)
      ssdpName = selectToMarkerLast(chipIDremote, "/");
      ssdpName = selectToMarker(ssdpName, "\r");
      //Serial.println(ssdpName);
      //ssdpName = selectToMarker(ssdpName, "/");
      //Serial.println(ssdpName);
      chipIDremote = selectToMarkerLast(chipIDremote, "/");
      chipIDremote = selectToMarker(chipIDremote, "\r");
      // строку input_string сохраняю для расширения
      ssdpList = jsonWrite(ssdpList, chipIDremote, udp.remoteIP().toString());
      ssdpLists(chipIDremote, udp.remoteIP().toString(), ssdpName);
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
  //list.printTo(Serial);
}

// Каждые 30 секунд проверяем не изиенился ли адрес ip
void ipChanges() {
  String ip = WiFi.localIP().toString();
  if (jsonRead(configSetup, "ip") != ip) {
    //configJson = jsonWrite(configJson, "ip", ip);
    configSetup = jsonWrite(configSetup, "ip", ip);
    requestSSDP();

  }
}


String xmlNode(String tags, String data) {
  String temp = "<" + tags + ">" + data + "</" + tags + ">";
  return temp;
}

String decToHex(uint32_t decValue, byte desiredStringLength) {
  String hexString = String(decValue, HEX);
  while (hexString.length() < desiredStringLength) hexString = "0" + hexString;
  return hexString;
}

