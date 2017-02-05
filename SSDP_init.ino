void SSDP_init(void) {
  //Если версия  2.0.0 закаментируйте следующую строчку
  SSDP.setDeviceType("upnp:rootdevice");
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort(80);
  SSDP.setName(ssdpName);
  SSDP.setSerialNumber("001788102201");
  SSDP.setURL("/");
  SSDP.setModelName("jalousie");
  SSDP.setModelNumber("000000000001");
  SSDP.setModelURL("http://esp8266-arduinoide.ru/jalousie/");
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
      Devices += ",{\"ip\":\"";
      Devices += udp.remoteIP().toString();
      Devices += "\"}";
      //Serial.println(Devices);
    }
  }
}

//void inquirySSDP() {
//  //  Инициализация поиска всех устройств в сети по протоколу SSDP ответ приходит от функции handleUDP
//  // Создаем адрес для широковещательного запроса
//  IPAddress ssdpAdress = WiFi.localIP();
//  ssdpAdress[3] = 255;
//  // Очишаем переменную со списком устройств и модулей
//  //Devices = "";
//  // Строка для запроса где на который ответят все устройства с SSDP протоколом
//  char  ReplyBuffer[] = "M-SEARCH * HTTP/1.1\r\nHost:239.255.255.250:1900\r\nST:upnp:rootdevice\r\nMan:\"ssdp:discover\"\r\nMX:3\r\n\r\n";
//  // Отправляем запрос ответ будем ждать в основном цыкле Loop handleUDP
//  udp.beginPacket(ssdpAdress, ssdpPort);
//  udp.write(ReplyBuffer);
//  udp.endPacket();
//}
//
//void handleUDP() {
//  String input_string = "";
//  char packetBuffer[255]; //buffer to hold incoming packet
//  int packetSize = udp.parsePacket();
//  if (packetSize) {
//    int len = udp.read(packetBuffer, 255);
//    if (len > 0) packetBuffer[len] = 0;
//    input_string += packetBuffer;
//    //Serial.println(input_string);
//    // Если в ответе устройства есть слово Arduino получит IP адрес
//    if (input_string.indexOf("Arduino") > 0) {
//      IPAddress remoteIp = udp.remoteIP();
//      // Хотим узнать какие модули работают на этом устройстве отправляем запрос на найденый IP
//      String urls = "http://" + udp.remoteIP().toString() + "/modules.json";
//      HTTPClient http;
//      http.begin(urls); //HTTP
//      int httpCode = http.GET();
//      if (httpCode == HTTP_CODE_OK) {
//        Devices += http.getString();
//        Devices += ",";
//      }
//      http.end();
//    }
//  }
//  // В строке Device теперь есть список устройств
//}




