void SSDP_init(void) {
    //Если версия  2.0.0 закаментируйте следующую строчку
  SSDP.setDeviceType("upnp:rootdevice");
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort(80);
  SSDP.setName(SSDP_Name);
  SSDP.setSerialNumber("001788102201");
  SSDP.setURL("/");
  SSDP.setModelName("Sonoff V2.0");
  SSDP.setModelNumber("000000000001");
  SSDP.setModelURL("http://esp8266-arduinoide.ru/sonoff/");
  SSDP.setManufacturer("Tretyakov Sergey");
  SSDP.setManufacturerURL("http://www.esp8266-arduinoide.ru");
  SSDP.begin();
}

void inquirySSDP() {
  IPAddress ssdpAdress = WiFi.localIP();
  ssdpAdress[3] = 255;
  Devices = "";
  char  ReplyBuffer[] = "M-SEARCH * HTTP/1.1\r\nHost:239.255.255.250:1900\r\nST:upnp:rootdevice\r\nMan:\"ssdp:discover\"\r\nMX:3\r\n\r\n";
  // send a reply, to the IP address and port that sent us the packet we received
  udp.beginPacket(ssdpAdress, ssdpPort);
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
      Devices += "<ip>";
      Devices += udp.remoteIP().toString();
      Devices += "</ip>";
      Devices += "\r\n";
      //Serial.println(Devices);
    }
  }
}
