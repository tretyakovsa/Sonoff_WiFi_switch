void initWIFI() {
  startWIFI();
  HTTP.on("/wifi.scan.json", HTTP_GET, []() {
    uint8_t n = WiFi.scanNetworks();
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    JsonArray& networks = json.createNestedArray("networks");
    for (uint8_t i = 0; i < n; i++) {
      JsonObject& data = networks.createNestedObject();
      data["ssid"] = WiFi.SSID(i);
      data["pass"] = (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "" : "*";
      data["dbm"] = WiFi.RSSI(i);
    }
    String root;
    json.printTo(root);
    httpOkJson(root);
  });
  HTTP.on("/ssid", HTTP_GET, []() {
    jsonWrite(configSetup, ssidS, HTTP.arg(ssidS));
    jsonWrite(configSetup, ssidPassS, HTTP.arg(ssidPassS));
    jsonWrite(configSetup, subnetS, HTTP.arg(subnetS));
    jsonWrite(configSetup, getwayS, HTTP.arg(getwayS));
    jsonWrite(configSetup, dnsS, HTTP.arg(dnsS));
    jsonWrite(configSetup, ipS, HTTP.arg(ipS));
    jsonWrite(configSetup, checkboxIPS, HTTP.arg(checkboxIPS));

    httpOkText();
    saveConfigSetup();
  });
  HTTP.on("/wifi", HTTP_GET, []() {
    jsonWrite(configSetup, wifiConnectS, HTTP.arg("connect"));
    jsonWrite(configSetup, wifiBlinkS, HTTP.arg("blink"));
    httpOkText();
    saveConfigSetup();
  });
  HTTP.on("/ssidap", HTTP_GET, []() {
    jsonWrite(configSetup, ssidAPS, HTTP.arg(ssidAPS));
    jsonWrite(configSetup, ssidApPassS, HTTP.arg(ssidApPassS));
    httpOkText();
    saveConfigSetup();
  });
  HTTP.on("/restart", HTTP_GET, []() {
    String restart = HTTP.arg("device");
    if (restart == "ok") {                         // Если значение равно Ок
      HTTP.send(200, "text/plain", "Reset OK"); // Oтправляем ответ Reset OK
      ESP.restart();                                // перезагружаем модуль
    } else {                                        // иначе
      HTTP.send(200, "text/plain", "No Reset"); // Oтправляем ответ No Reset
    }
  });
  HTTP.on("/restartWiFi", HTTP_GET, []() {
    WiFi.mode(WIFI_AP_STA);
    // Не отключаясь от точки доступа подключаемся к роутеру для получения будущего IP
    String ssid = jsonRead(configSetup, ssidS);
    String pass = jsonRead(configSetup, ssidPassS);
    Serial.println(ssid);
    Serial.println(pass);
    WiFi.begin(ssid.c_str(), pass.c_str());
    wifiConnect(jsonReadtoInt(configSetup, wifiConnectS), jsonReadtoInt(configSetup, wifiBlinkS));
    // {"title":"<h3>{{LangConnect2}} <a href="http://192.168.1.30">http://192.168.1.30</a></h3>"}
    // {"title":"Любой текст и html","class":"класс", "style":"Стиль","state":"Данные для вставки в state input_а"}
    String state = "\{\"title\":\"<h3>\{\{LangConnect2\}\} <a href=http://" + WiFi.localIP().toString() + ">http://" + WiFi.localIP().toString() + "</a></h3>\"\}";
    HTTP.send(200, "application/json", state);
    delay(1000);
    // Отключаем точку доступа и переподключаемся к роутеру
    ESP.restart();
  });
}

// --------------Проверять на подключение к роутеру  tries раз. Отображать подключение на ноге pin
boolean wifiConnect(byte tries, byte pin) {
  if (tries == 0) tries = 11;
  pin = pinTest(pin);
  if (pin != 17)   pinMode(pin, OUTPUT);
  while (--tries && WiFi.status() != WL_CONNECTED)
  {
    //Мигаем сетодиодом при попытке подключится к роутеру
    if (pin != 0)   digitalWrite(pin, HIGH);
    delay(500);
    if (pin != 0)  digitalWrite(pin, LOW);
    delay(500);
  }
  if (WiFi.status() == WL_CONNECTED)return true;
  return false;
}

// ----------------- Запускаем WiFi
void startWIFI() {
  if (startSTA()) {
  }
  else {
    startAP();
  }
}

//-------------Включение режима AP конфигурация в строке json
boolean startAP() {
  IPAddress apIP(192, 168, 4, 1);
  IPAddress staticGateway(192, 168, 4, 1);
  IPAddress staticSubnet(255, 255, 255, 0);
  WiFi.softAPConfig(apIP, staticGateway, staticSubnet);
  WiFi.softAP(jsonRead(configSetup, ssidAPS).c_str(), jsonRead(configSetup, ssidApPassS).c_str());
  WiFi.mode(WIFI_AP);
  dnsServer.start(53, "*", apIP);
  digitalWrite(jsonReadtoInt(configSetup, wifiBlinkS), HIGH);                 //Зажигаем светодиод если находимся в режиме AP
  return true;
}

//-------------Подключение к роутеру конфигурация в строке json
boolean startSTA() {
  if (jsonRead(configSetup, checkboxIPS) == "1") {
    IPAddress staticIP;
    IPAddress staticGateway;
    IPAddress staticSubnet;
    String volume = "";
    boolean check = true;
    volume = jsonRead(configSetup, ipS);
    if (volume != "") {
      staticIP.fromString(volume);
      check = false;
    }
    volume = jsonRead(configSetup, getwayS);
    if (volume != "") {
      staticGateway.fromString(volume);
      check = false;
    }
    volume = jsonRead(configSetup, subnetS);
    if (volume != "") {
      staticSubnet.fromString(volume);
      check = false;
    }
    if (!check) {
      WiFi.config (staticIP, staticGateway, staticSubnet);
    }
  }
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.hostname ( "sonoff-" + chipID );
  WiFi.begin();
  //WiFi.begin(jsonRead(configJson, ssidS).c_str(),jsonRead(configJson, ssidPassS).c_str());
  if ( wifiConnect(jsonReadtoInt(configSetup, wifiConnectS), jsonReadtoInt(configSetup, wifiBlinkS))) {
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    jsonWrite(configSetup, ipS, WiFi.localIP().toString());
    jsonWrite(configSetup, getwayS, WiFi.gatewayIP().toString());
    jsonWrite(configSetup, subnetS, WiFi.subnetMask().toString());
    statistics();
    return true;
  }
  return false;
}


