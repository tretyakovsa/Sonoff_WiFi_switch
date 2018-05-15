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
    sendSetupArg(ssidS);
    sendSetupArg(ssidPassS);
    sendSetupArg(subnetS);
    sendSetupArg(getwayS);
    sendSetupArg(dnsS);
    sendSetupArg(ipS);
    sendSetupArg(checkboxIPS);
    httpOkText();
    saveConfigSetup();
  });
   HTTP.on("/wifi", HTTP_GET, []() {
    sendSetup(wifiConnectS, HTTP.arg("connect"));
    sendSetup(wifiBlinkS, HTTP.arg("blink"));
    httpOkText();
    saveConfigSetup();
  });
  HTTP.on("/ssidap", HTTP_GET, []() {
    sendSetupArg(ssidAPS);
    sendSetupArg(ssidApPassS);
    httpOkText();
    saveConfigSetup();
  });
  HTTP.on("/restart", HTTP_GET, []() {
    if (HTTP.arg("device") == "ok") {               // Если значение равно Ок
      httpOkText("Reset OK"); // Oтправляем ответ Reset OK
      ESP.restart();                                // перезагружаем модуль
    } else {                                        // иначе
      httpOkText("No Reset"); // Oтправляем ответ No Reset
    }
  });
   HTTP.on("/restartWiFi", HTTP_POST, []() {
    WiFi.mode(WIFI_AP_STA);
    // Не отключаясь от точки доступа подключаемся к роутеру для получения будущего IP
    String ssid = getSetup(ssidS);
    String pass = getSetup(ssidPassS);
    WiFi.begin(ssid.c_str(), pass.c_str());
    wifiConnect();
    String state = "\{\"title\":\"<h3>\{\{LangConnect2\}\} <a href=http://" + WiFi.localIP().toString() + ">http://" + WiFi.localIP().toString() + "</a></h3>\"\}";
    httpOkJson(state);
    delay(1000);
    // Отключаем точку доступа и переподключаемся к роутеру
    ESP.restart();
  });
}

void sendSetupArg(String argS){
  sendSetup(argS, HTTP.arg(argS));
  }
// ----------------- Запускаем WiFi
void startWIFI() {
  if (startSTA()) {
  }
  else {
    startAP();
  }
}

boolean startSTA() {
  String volume; // ---------Настроим статические параметры
  boolean check = true;
  if (getSetupInt(checkboxIPS)) {
    IPAddress staticIP;
    volume = getSetup(ipS);
    if (volume != emptyS) {
      staticIP.fromString(volume);
      check = false;
    }
    IPAddress staticGateway;
    volume = getSetup(getwayS);
    if (volume != emptyS) {
      staticGateway.fromString(volume);
      check = false;
    }
    IPAddress staticSubnet;
    volume = getSetup(subnetS);
    if (volume != emptyS) {
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
  String ssid = getSetup(ssidS);
  String pass = getSetup(ssidPassS);
  WiFi.persistent(false);
  WiFi.begin(ssid.c_str(), pass.c_str());
  if ( wifiConnect()) {
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    sendSetup(ipS,WiFi.localIP().toString());
    sendSetup(getwayS,WiFi.gatewayIP().toString());
    sendSetup(subnetS,WiFi.subnetMask().toString());
    statistics();
    return true;
  }
  return false;
}
boolean wifiConnect() {
  uint8_t tries = getSetupInt(wifiConnectS);
  if (tries == 0) tries = 11;
    uint8_t pin = pinTest(getSetupInt(wifiBlinkS));
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

//-------------Включение режима AP конфигурация в строке configSetup
boolean startAP() {
  IPAddress apIP(192, 168, 4, 1);
  IPAddress staticGateway(192, 168, 4, 1);
  IPAddress staticSubnet(255, 255, 255, 0);
  WiFi.softAPConfig(apIP, staticGateway, staticSubnet);
  WiFi.softAP(getSetup(ssidAPS).c_str(), getSetup(ssidApPassS).c_str());
  WiFi.mode(WIFI_AP);
  dnsServer.start(53, "*", apIP);
  digitalWrite(getSetupInt(wifiBlinkS), HIGH);                 //Зажигаем светодиод если находимся в режиме AP
  jsonWrite(ssdpList, getSetup(ssdpS), "0.0.0.0");
  return true;
}
