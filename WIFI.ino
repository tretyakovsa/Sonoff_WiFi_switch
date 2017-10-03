//Подключение к сети в режиме станции при неудачной попытки включим режим точки доступа
/* 1. Проверяем есть ли точка доступа установленная во flash
    Если да:
    проверяем режим DHCP или Manual IP
    Если Manual IP устанавливаем парамерты
    Иначе делаем попытку подключения к точке доступа
    Точка доступа не доступна Вклчаем режим AP
*/
// ---------------- Инициализация WiFi
void initWIFI() {
  // сканирование сети на доступные точки доступа
  HTTP.on("/wifi.scan.json", HTTP_GET, [](AsyncWebServerRequest * request) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    JsonArray& networks = json.createNestedArray("networks");
    int n = WiFi.scanComplete();
    if (n == -2) {
      WiFi.scanNetworks(true);
    } else if (n) {
      for (int i = 0; i < n; i++) {
        JsonObject& data = networks.createNestedObject();
        data["ssid"] = WiFi.SSID(i);
        data["pass"] = (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "" : "*";
        data["dbm"] = WiFi.RSSI(i);
      }
      WiFi.scanDelete();
      if (WiFi.scanComplete() == -2) {
        WiFi.scanNetworks(true);
      }
    }
    String root;
    json.printTo(root);
    request->send(200, "application/json", root);
  });
  // --------------Установить имя и пароль для роутера
  HTTP.on("/ssid", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasArg("ssid")) {
      configSetup = jsonWrite(configSetup, "ssid", request->arg("ssid"));
    }
    if (request->hasArg("password")) {
      configSetup = jsonWrite(configSetup, "ssidPass", request->arg("password"));
    }
    if (request->hasArg("checkboxIP")) {
      configSetup = jsonWrite(configSetup, "checkboxIP", request->arg("checkboxIP"));
    }
    if (request->hasArg("ip")) {
      configSetup = jsonWrite(configSetup, "ip", request->arg("ip"));
    }
    if (request->hasArg("subnet")) {
      configSetup = jsonWrite(configSetup, "subnet", request->arg("subnet"));
    }
    if (request->hasArg("getway")) {
      configSetup = jsonWrite(configSetup, "getway", request->arg("getway"));
    }
    request->send(200, "text/plain", "Ok");
    saveConfigSetup();
  });
  // --------------Установить led и попытки для wifi
  HTTP.on("/wifi", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasArg("connect")) {
      configSetup = jsonWrite(configSetup, "wifiConnect", request->arg("connect"));
    }
    if (request->hasArg("blink")) {
      configSetup = jsonWrite(configSetup, "wifiBlink", request->arg("blink"));
    }
    request->send(200, "text/plain", "Ok");
    saveConfigSetup();
  });
  // --------------- Установить имя и пароль для AP
  HTTP.on("/ssidap", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasArg("ssidAP")) {
      configSetup = jsonWrite(configSetup, "ssidAP", request->arg("ssidAP"));
    }
    if (request->hasArg("passwordAP")) {
      configSetup = jsonWrite(configSetup, "passwordAP", request->arg("passwordAP"));
    }
    request->send(200, "text/plain", "Ok");
    saveConfigSetup();
  });
  // -------------- Перезагрузка модуля
  HTTP.on("/restart", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasArg("device")) {
      if (request->arg("device") == "ok") {                         // Если значение равно Ок
        request->send(200, "text / plain", "Reset OK"); // Oтправляем ответ Reset OK
        ESP.restart();                                // перезагружаем модуль
      }
      else {                                        // иначе
        request->send(200, "text / plain", "No Reset"); // Oтправляем ответ No Reset
      }
    }
  });

  // -------------- Перезагрузка модуля
  HTTP.on("/restartWiFi", HTTP_POST, [](AsyncWebServerRequest * request) {
    //Перезапуск Wi-Fi при первой настройке
    Serial.println("WiFi reconnect");
    WiFi.mode(WIFI_AP_STA);
    // Не отключаясь от точки доступа подключаемся к роутеру для получения будущего IP
    WiFi.begin(jsonRead(configSetup, "ssid").c_str(), jsonRead(configSetup, "ssidPass").c_str());
    wifiConnect(jsonReadtoInt(configSetup, "wifiConnect"), jsonReadtoInt(configSetup, "wifiBlink"));
    // {"title":"<h3>{{LangConnect2}} <a href="http://192.168.1.30">http://192.168.1.30</a></h3>"}
    // {"title":"Любой текст и html","class":"класс", "style":"Стиль","state":"Данные для вставки в state input_а"}
    String state = "\{\"title\":\"<h3>\{\{LangConnect2\}\} <a href=http://" + WiFi.localIP().toString() + ">http://" + WiFi.localIP().toString() + "</a></h3>\"\}";
    request->send(200, "application/json", state);
    // Отключаем точку доступа и переподключаемся к роутеру
    ESP.restart();
  });
  startWIFI();
}

//-------------Подключение к роутеру конфигурация в строке json
//{"ip":"192.168.0.190","getway":"192.168.0.1","subnet":"255.255.255.0","dns":"","checkboxIP":0}
boolean startSTA(String configWiFi) {
  //WiFi.persistent (false);
  if (jsonRead(configWiFi, "checkboxIP") == "1") {
    Serial.println ("checkboxIP");
    IPAddress staticIP;
    IPAddress staticGateway;
    IPAddress staticSubnet;
    String volume = "";
    boolean check = true;
    volume = jsonRead(configWiFi, "ip");
    if (volume != "") {
      staticIP.fromString(volume);
      check = false;
    }
    volume = jsonRead(configWiFi, "getway");
    if (volume != "") {
      staticGateway.fromString(volume);
      check = false;
    }
    volume = jsonRead(configWiFi, "subnet");
    if (volume != "") {
      staticSubnet.fromString(volume);
      check = false;
    }
    if (!check) {
      Serial.println ("check");
      Serial.println (WiFi.config(staticIP, staticGateway, staticSubnet));
      Serial.println (WiFi.subnetMask ());
      Serial.println (WiFi.gatewayIP ());
    }
  }
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.hostname ( "sonoff-" + chipID );
  Serial.println(WiFi.SSID());
  WiFi.begin();
  //WiFi.begin(jsonRead(configJson, "ssid").c_str(),jsonRead(configJson, "ssidPass").c_str());
  if ( wifiConnect(jsonReadtoInt(configSetup, "wifiConnect"), jsonReadtoInt(configSetup, "wifiBlink"))) {
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    configSetup = jsonWrite(configSetup, "ip", WiFi.localIP().toString());
    configSetup = jsonWrite(configSetup, "getway", WiFi.gatewayIP().toString());
    configSetup = jsonWrite(configSetup, "subnet", WiFi.subnetMask().toString());
    statistics();
    return true;
  }
  return false;
}
// --------------Проверять на подключение к роутеру  tries раз. Отображать подключение на ноге pin
boolean wifiConnect(byte tries, byte pin) {
  if (tries == 0) tries = 11;
  if ((pin > 5 && pin < 12) || pin > 16) pin = 0 ;
  if (pin != 0)   pinMode(pin, OUTPUT);
  while (--tries && WiFi.status() != WL_CONNECTED)
  {
    //Мигаем сетодиодом при попытке подключится к роутеру
    if (pin != 0)   digitalWrite(pin, HIGH);
    Serial.print(".");
    delay(500);
    if (pin != 0)  digitalWrite(pin, LOW);
    delay(500);
  }
  if (WiFi.status() == WL_CONNECTED)return true;
  return false;
}

//-------------Включение режима AP конфигурация в строке json
//{"ssidAP":"Sonoff","ssidApPass":""}
boolean startAP(String configWiFi) {
  IPAddress apIP(192, 168, 4, 1);
  IPAddress staticGateway(192, 168, 4, 1);
  IPAddress staticSubnet(255, 255, 255, 0);
  WiFi.softAPConfig(apIP, staticGateway, staticSubnet);
  WiFi.softAP(jsonRead(configWiFi, "ssidAP").c_str(), jsonRead(configWiFi, "ssidApPass").c_str());
  WiFi.mode(WIFI_AP);
  dnsServer.start(53, "*", apIP);
  //Зажигаем светодиод если находимся в режиме AP
  int pin = jsonReadtoInt(configWiFi, "blink");
  if ((pin > 5 && pin < 12) || pin > 16) pin = 0 ;
  if (pin != 0)  {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
  }
  return true;
}



// ----------------- Запускаем WiFi
void startWIFI() {
  if (startSTA(configSetup)) {
    //Serial.println(WiFi.localIP().toString());
  }
  else {
    startAP(configSetup);
    //Serial.println("Start AP");
  }
}
