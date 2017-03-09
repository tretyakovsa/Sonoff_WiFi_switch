void WiFi_init() {
  pinMode(LED_PIN, OUTPUT);
  HTTP.on("/wifi.scan.json", handle_wifi_scan);      // сканирование сети на доступные точки доступа
  HTTP.on("/ssid", handle_ssid);        // Установить имя и пароль роутера
  HTTP.on("/ssidap", handle_ssidap);    // Установить имя и пароль для точки доступа
  HTTP.on("/restartWiFi", RestartWiFi);                // Перизапустить wifi попытаться узнать будущий ip адрес перезагрузить устройство
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
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

//Установить имя и пароль для точки доступа
void handle_ssidap() {
  ssidApName = HTTP.arg("ssidAP");
  ssidApPass = HTTP.arg("ssidApPass");
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}


void WIFIAP_Client() {
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssidName.c_str(), ssidPass.c_str());
  tries(11);
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
      urls +=String(mac[5], HEX);
      urls +=String(mac[4], HEX);
      urls +=String(mac[3], HEX);
      urls +=String(mac[2], HEX);
      urls +=String(mac[1], HEX);
      urls +=String(mac[0], HEX);
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

