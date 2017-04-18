void SSDP_init() {
  // Закускаем UDP для поиска устройств
  udp.begin(localPort);
  // /device?ssdp=Sonoff-Rele&space={{space}}
  HTTP.on("/device", handle_ssdp);        // Установить имя устройства  HTTP.on("/ssdp", handle_ssdp);        // Установить имя устройства
  HTTP.on("/devices.scan.json", inquirySSDP);  // запустить поиск устройств по ssdp
  // SSDP дескриптор
  HTTP.on("/description.xml", HTTP_GET, []() {
    SSDP.schema(HTTP.client());
  });
  //Если версия  2.0.0 закаментируйте следующую строчку
  SSDP.setDeviceType("upnp:rootdevice");
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort(80);
  SSDP.setName(ssdpName);
  SSDP.setSerialNumber(chipID);
  SSDP.setURL("/index.htm");
  SSDP.setModelName("sonoff");
  SSDP.setModelNumber("000000000001");
  SSDP.setModelURL("https://github.com/tretyakovsa/Sonoff_WiFi_switch");
  SSDP.setManufacturer("Tretyakov Sergey, Kevrels Renats");
  SSDP.setManufacturerURL("http://www.esp8266-arduinoide.ru");
  SSDP.begin();
}

// Установить имя устройства
void handle_ssdp() {
  // /device?ssdp=Sonoff-Rele&space={{space}}
  ssdpName = HTTP.arg("ssdp");
  spaceName = HTTP.arg("space");
  SSDP.setName(ssdpName);
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

// запустить поиск устройств по ssdp
void inquirySSDP() {
  IPAddress ssdpAdress = WiFi.localIP();
  ssdpAdress[0] = 239;
  ssdpAdress[1] = 255;
  ssdpAdress[2] = 255;
  ssdpAdress[3] = 250;
  //IPAddress ssdpAdress(239,255,255,250);
  Devices = "";
  char  ReplyBuffer[] = "M-SEARCH * HTTP/1.1\r\nHost:239.255.255.250:1900\r\nST:upnp:rootdevice\r\nMan:\"ssdp:discover\"\r\nMX:3\r\n\r\n";
  // send a reply, to the IP address and port that sent us the packet we received
  udp.beginPacket(ssdpAdress, ssdpPort);
  //udp.beginPacketMulticast(ssdpAdress, ssdpPort);
  udp.write(ReplyBuffer);
  udp.endPacket();
  HTTP.send(200, "text/plain", "OK");
}

void handleUDP() {
  String input_string = "";
  char packetBuffer[255]; //buffer to hold incoming packet
  // if there's data available, read a packet
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len] = 0;
    input_string += packetBuffer;
    if (input_string.indexOf("Arduino") > 0) {
      IPAddress remoteIp = udp.remoteIP();
        Serial.println(input_string);
      // Хотим узнать какие модули работают на этом устройстве отправляем запрос на найденый IP
      String urls = "http://" + udp.remoteIP().toString() + "/modules.json";
      HTTPClient http;
      http.begin(urls); //HTTP
      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK) {
        Devices += ",";
        Devices += http.getString();
      }
      http.end();
    }
  }
}
