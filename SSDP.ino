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
  SSDP.setModelNumber(chipID);
  SSDP.setModelURL("https://github.com/tretyakovsa/Sonoff_WiFi_switch");
  SSDP.setManufacturer("Tretyakov Sergey, Kevrels Renats");
  SSDP.setManufacturerURL("http://www.esp8266-arduinoide.ru");
  SSDP.begin();
  modulesReg("device");
  sCmd.addCommand("device", device);
  sCmd.addCommand("P",     processCommand);
  sCmd.setDefaultHandler(unrecognized);

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
  Devices = "";
  Serial.println(DevicesList);
  devicesGet(DevicesList);

  HTTP.send(200, "text/plain", Devices);
}


// запустить поиск устройств по ssdp
void searchSSDP() {
  IPAddress ssdpAdress = WiFi.localIP();
  ssdpAdress[0] = 239;
  ssdpAdress[1] = 255;
  ssdpAdress[2] = 255;
  ssdpAdress[3] = 250;
  //IPAddress ssdpAdress(239,255,255,250);

  DevicesList = "";
  char  ReplyBuffer[] = "M-SEARCH * HTTP/1.1\r\nHost:239.255.255.250:1900\r\nST:upnp:rootdevice\r\nMan:\"ssdp:discover\"\r\nMX:3\r\n\r\n";
  // send a reply, to the IP address and port that sent us the packet we received
  udp.beginPacket(ssdpAdress, ssdpPort);
  //udp.beginPacketMulticast(ssdpAdress, ssdpPort);
  udp.write(ReplyBuffer);
  udp.endPacket();
}

void handleUDP() {
  String input_string = "";
  char packetBuffer[512]; //buffer to hold incoming packet
  // if there's data available, read a packet
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(packetBuffer, 512);
    if (len > 0) packetBuffer[len] = 0;
    input_string += packetBuffer;
    if (input_string.indexOf("Arduino") > 0) {
      IPAddress remoteIp = udp.remoteIP();
      //Serial.println(input_string);
      // Хотим узнать какие модули работают на этом устройстве отправляем запрос на найденый IP
      DevicesList += udp.remoteIP().toString() + ",";

    }
  }
}

// Получаем список модулей для запроса /devices.scan.json

void devicesGet(String ipList) {
  if (ipList != "") {
    String list = ipList;
    do {
      String ipNext = subString (list, ",");
      Serial.println(ipNext);


      // Хотим узнать какие модули работают на этом устройстве отправляем запрос на найденый IP
      String urls = "http://" + ipNext + "/modules.json";

      HTTPClient http;
      http.begin(urls); //HTTP
      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK) {
        Devices += ",";
        Devices += http.getString();
      }
      http.end();

      list = stringTrim(list, ",");
      //Serial.println(list);

    } while  (list != "");
  }
}



void device() {
  //  ssdp=Smart&space=test&

  char *arg;
  //arg = sCmd.next();    // Get the next argument from the SerialCommand object buffer
  //Serial.println(arg);
  do  {    // As long as it existed, take it
    arg = sCmd.next();
    if (arg == NULL) return;
    String temp = findParm(arg, "ssdp");
    if (temp != "") ssdpName = temp;

    delay(1);
    //temp=findParm(arg, "space");
    //if(temp!="") spaceName=temp;
    //delay(1);
  } while (arg != NULL);
  Serial.println(ssdpName);
  Serial.println(spaceName);
}

String findParm(String str, String found) {
  if (subString (str, "=") == found) {
    return stringTrim(str, "=");
  }
}

void processCommand() {
  String aNumber;
  char *arg;

  Serial.println("We're in processCommand");
  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = arg;    // Converts a char string to an integer
    Serial.print("First argument was: ");
    Serial.println(aNumber);
  }
  else {
    Serial.println("No arguments");
  }

  arg = sCmd.next();
  if (arg != NULL) {
    aNumber = arg;
    Serial.print("Second argument was: ");
    Serial.println(aNumber);
  }
  else {
    Serial.println("No second argument");
  }
}

// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.println("What?");
}
