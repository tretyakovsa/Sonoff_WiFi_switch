void WiFi_init() {
  pinMode(LED_PIN, OUTPUT);
  HTTP.on("/wifi.scan.json", handle_wifi_scan);      // сканирование сети на доступные точки доступа
  HTTP.on("/ssid", handle_ssid);        // Установить имя и пароль роутера
  HTTP.on("/ssidap", handle_ssidap);    // Установить имя и пароль для точки доступа
  //ttp://192.168.0.100/ssid?ssid=home&ssidPass=i12345678&subnet=255.255.255.0&getway=192.168.0.1&dns=192.168.0.1&ip=192.168.0.99&checkboxIP=1
  HTTP.on("/restartWiFi", RestartWiFi);                // Перизапустить wifi попытаться узнать будущий ip адрес перезагрузить устройство
  ip_ConfigJSON();
  WIFIAP_Client();
}
// сканирование сети на доступные точки доступа
void handle_wifi_scan() {
  int n = WiFi.scanNetworks();
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  JsonArray& networks = json.createNestedArray("networks");
  for (int i = 0; i < n; i++) {
    JsonObject& data = networks.createNestedObject();
    data["ssid"] = WiFi.SSID(i);
    data["pass"] = (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "" : "*";
    data["dbm"] = WiFi.RSSI(i);
    //data["bssid"] = WiFi.BSSIDstr(i);
    //data["channel"] = WiFi.channel(i);
    //data["isHidden"] = WiFi.isHidden(i);
  }
  String root;
  json.printTo(root);
  HTTP.send(200, "text/json", root);
}

// Установить имя и пароль для роутера
void handle_ssid() {
  ssidName = HTTP.arg("ssid");
  ssidPass = HTTP.arg("ssidPass");
  subnet = HTTP.arg("subnet");
  getway = HTTP.arg("getway");
  dns = HTTP.arg("dns");
  ip = HTTP.arg("ip");
  checkboxIP = HTTP.arg("checkboxIP").toInt();
  saveConfig();
  ip_ConfigJSON();
  HTTP.send(200, "text/plain", "OK");
}

//Установить имя и пароль для точки доступа
void handle_ssidap() {
  ssidApName = HTTP.arg("ssidAP");
  ssidApPass = HTTP.arg("ssidApPass");
  saveConfig();

  // Резервируем память для json обекта буфер может рости по мере необходимти, предпочтительно для ESP8266
  DynamicJsonBuffer jsonBuffer;
  //  вызовите парсер JSON через экземпляр jsonBuffer
  JsonObject& json = jsonBuffer.parseObject(jsonConfig);
  // Заполняем поля json
  json["ssidAP"] = ssidApName;
  json["ssidApPass"] = ssidApPass;
  // Помещаем созданный json в переменную root
  jsonConfig = "";
  json.printTo(jsonConfig);
  Serial.println(jsonConfig);
  HTTP.send(200, "text/plain", "OK");
}
void ip_ConfigJSON() {
  // Резервируем память для json обекта буфер может рости по мере необходимти, предпочтительно для ESP8266
  DynamicJsonBuffer jsonBuffer;
  //  вызовите парсер JSON через экземпляр jsonBuffer
  JsonObject& json = jsonBuffer.parseObject(jsonConfig);
  // Заполняем поля json
  json["ssidAP"] = ssidApName;
  json["ssidApPass"] = ssidApPass;
  json["ssid"] = ssidName;
  json["ssidPass"] = ssidPass;
  json["subnet"] = subnet;
  json["getway"] = getway;
  json["dns"] = dns;
  json["checkboxIP"] = checkboxIP;

  // Помещаем созданный json в переменную root
  jsonConfig = "";
  json.printTo(jsonConfig);
  Serial.println(jsonConfig);
  HTTP.send(200, "text/json", jsonConfig);
}


void WIFIAP_Client() {

  IPAddress staticIP;
  IPAddress staticGateway;
  IPAddress staticSubnet;
  //bool fromString(const String &address) { return fromString(address.c_str()); }
  WiFi.disconnect();
  WiFi.hostname ( "sonoff" );
  WiFi.mode(WIFI_STA);
  if (checkboxIP == 1) {
    staticIP.fromString(ip);
    staticGateway.fromString(getway);
    staticSubnet.fromString(subnet);
    WiFi.config(staticIP, staticGateway, staticSubnet);
  }
  WiFi.begin(ssidName.c_str(), ssidPass.c_str());
  tries(11);
  WiFi.setAutoReconnect(true);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WL_CONNECTED);
  Serial.println("IP address: ");
  //ipCurrent = WiFi.localIP().toString();
  Serial.println(WiFi.localIP().toString());
  if (WiFi.status() != WL_CONNECTED)
  {
    StartAPMode();
  }
  String urls = "http://backup.privet.lv/visitors.php?";
  byte mac[6];
  WiFi.macAddress(mac);
  urls += String(mac[5], HEX);
  urls += String(mac[4], HEX);
  urls += String(mac[3], HEX);
  urls += String(mac[2], HEX);
  urls += String(mac[1], HEX);
  urls += String(mac[0], HEX);
  HTTPClient http;
  http.begin(urls); //HTTP
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    Serial.println(http.getString());
  }
  http.end();
}

bool StartAPMode()
{
  const byte DNS_PORT = 53;
  IPAddress apIP(192, 168, 4, 1);
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssidApName.c_str(), ssidApPass.c_str());
  dnsServer.start(DNS_PORT, "*", apIP);
  //Зажигаем светодиод если находимся в режиме AP
  digitalWrite(LED_PIN, HIGH);
  return true;
}

// Перизапустить wifi попытаться узнать будущий ip адрес перезагрузить устройство
bool RestartWiFi() {
  //Холодный перезапуск Wi-Fi при первой настройке
  Serial.println("WiFi reconnect");
  // Не отключаясь точки доступа подключаемся к роутеру для получения будущего IP
  WiFi.mode(WIFI_AP_STA );
  WiFi.begin(ssidName.c_str(), ssidPass.c_str());
  tries(30);

  Serial.println("");
  Serial.println(WiFi.localIP());
  HTTP.send(200, "text/plain", WiFi.localIP().toString());
  delay(5000);
  // Отключаем точку доступа и переподключаемся к роутеру
  /*WiFi.mode(WIFI_STA);
    WiFi.begin();
    tries(11);
  */
  ESP.restart();
}

// Попытки подключения к сети
void tries(byte tries) {
  while (--tries && WiFi.status() != WL_CONNECTED)
  {
    //Мигаем сетодиодом при попытке подключится к роутеру
    digitalWrite(LED_PIN, HIGH);
    Serial.print(".");
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);

  }
}


// Выделяем строку до маркера
String subString (String str, String found) {
  int p = str.indexOf(found);
  return str.substring(0, p);
}
//Удоляем все до символа разделителя
String stringTrim(String str, String found) {
  int p = str.indexOf(found) + found.length();
  return str.substring(p);
}
