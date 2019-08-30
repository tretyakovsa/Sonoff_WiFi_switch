// Сообщает статус wifi соединения
void WiFiEvent(WiFiEvent_t event) {
  //flag = sendStatus(wifiS, event);
  sendStatus(wifiS, event);
  switch (event) {
    case WIFI_EVENT_STAMODE_CONNECTED:
      //Serial.println("WIFI_EVENT_STAMODE_CONNECTED");
           sendSetup(ipS, WiFi.localIP().toString());
           sendSetup(getwayS, WiFi.gatewayIP().toString());
           sendSetup(subnetS, WiFi.subnetMask().toString());
           sendStatus(wifiS, event);
           //sendStatus(ipS, WiFi.localIP().toString());
      break;
    case WIFI_EVENT_STAMODE_AUTHMODE_CHANGE:
      //Serial.println("WIFI_EVENT_STAMODE_AUTHMODE_CHANGE");
      flag = sendStatus(wifiS, event);
      break;
    case WIFI_EVENT_STAMODE_GOT_IP:
     // Serial.println("WIFI_EVENT_STAMODE_GOT_IP");
     // Serial.println(WiFi.localIP().toString());
      flag = sendStatus(wifiS, event);
      break;
    case WIFI_EVENT_STAMODE_DISCONNECTED:
      //Serial.println("WIFI_EVENT_STAMODE_DISCONNECTED");
      flag = sendStatus(wifiS, event);
      break;
    case WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED:
     // Serial.println("WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED");
      flag = sendStatus(wifiS, event);
        sendSetup(ipS, WiFi.softAPIP().toString());
      //sendStatus(ipS, WiFi.softAPIP().toString());
      break;
  }
  // 0 WIFI_EVENT_STAMODE_CONNECTED    подключение к роутеру получение ip
  // 1 WIFI_EVENT_STAMODE_DISCONNECTED попытка переподключения к роутеру
  // 2 WIFI_EVENT_STAMODE_AUTHMODE_CHANGE
  // 3 WIFI_EVENT_STAMODE_GOT_IP подключен к роутеру
  // 4 WIFI_EVENT_STAMODE_DHCP_TIMEOUT Не получен адрес DHCP
  // 5 WIFI_EVENT_SOFTAPMODE_STACONNECTED подключен клент
  // 6 WIFI_EVENT_SOFTAPMODE_STADISCONNECTED отключен клент
  // 7 WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED Режим точки доступа
  // 8 WIFI_EVENT_MAX,
  // 9 WIFI_EVENT_ANY = WIFI_EVENT_MAX,
  // 10 WIFI_EVENT_MODE_CHANGE

}

String scanWIFI() {
  uint8_t n = WiFi.scanNetworks();
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  JsonArray& networks = json.createNestedArray("networks");
  for (uint8_t i = 0; i < n; i++) {
    JsonObject& data = networks.createNestedObject();
    String ssidMy = WiFi.SSID(i);
    data["ssid"] = ssidMy;
    data["pass"] = (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? emptyS : "*";
    int8_t dbm = WiFi.RSSI(i);
    data["dbm"] = dbm;
    if (ssidMy == getSetup(ssidS)) {
      sendStatus("dbm", dbm);
    }
  }
  String root;
  json.printTo(root);
  return root;
}

void initWIFI() {
  startWIFI();
  HTTP.on("/wifi.scan.json", HTTP_GET, []() {
    httpOkJson(scanWIFI());
  });
  // задача проверять уровень сети каждые две минуты.
  ts.add(tRSSI, 300000, [&](void*) {
    scanWIFI();
    //Serial.println(ssdpList);
    //Serial.println("requestSSDP "+GetTime());
  }, nullptr, true);
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
//restart?device=ok
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
    saveConfigSetup();
    WiFi.begin(ssid.c_str(), pass.c_str());
    wifiConnect();
    String state = "\{\"title\":\"<h3>\{\{LangConnect2\}\} <a href=http://" + WiFi.localIP().toString() + ">http://" + WiFi.localIP().toString() + "</a></h3>\"\}";
    httpOkJson(state);
    delay(1000);
    // Отключаем точку доступа и переподключаемся к роутеру
    ESP.restart();
  });
}

void sendSetupArg(String argS) {
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
  WiFi.onEvent(WiFiEvent);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  scanWIFI();
  WiFi.hostname ( "sonoff-" + chipID );
  String ssid = getSetup(ssidS);
  String pass = getSetup(ssidPassS);
  WiFi.persistent(false);
  if (ssid == emptyS && pass == emptyS) {
    WiFi.begin();
  }
  else {
    WiFi.begin(ssid.c_str(), pass.c_str());
  }
  if ( wifiConnect()) {
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    sendSetup(ipS, WiFi.localIP().toString());
    sendSetup(getwayS, WiFi.gatewayIP().toString());
    sendSetup(subnetS, WiFi.subnetMask().toString());
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
    //Serial.println("Recconnect");
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
  //statistics();
  return true;
}
