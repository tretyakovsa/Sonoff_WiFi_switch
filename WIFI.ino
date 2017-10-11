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
  HTTP.on("/wifi.scan.json", handle_wifi_scan);      // сканирование сети на доступные точки доступа
  HTTP.on("/ssid", handle_ssid);        // Установить имя и пароль роутера
  HTTP.on("/wifi", handle_wifi);    // Установить имя и пароль для точки доступа
  HTTP.on("/ssidap", handle_ssidap);    // Установить имя и пароль для точки доступа
  HTTP.on("/restart", handle_restart);               // Перезагрузка модуля
  HTTP.on("/restartWiFi", RestartWiFi);                // Перизапустить wifi попытаться узнать будущий ip адрес перезагрузить устройство
  startWIFI();
}

void handle_wifi(){
  configSetup = jsonWrite(configSetup, "wifiConnect", HTTP.arg("connect"));
  configSetup = jsonWrite(configSetup, "wifiBlink", HTTP.arg("blink"));
  HTTP.send(200, "text/plain", "Ok");
  saveConfigSetup();
  }

// --------------Установить имя и пароль для роутера
void handle_ssid() {
  configSetup = jsonWrite(configSetup, "ssid", HTTP.arg("ssid"));
  configSetup = jsonWrite(configSetup, "subnet", HTTP.arg("subnet"));
  configSetup = jsonWrite(configSetup, "getway", HTTP.arg("getway"));
  configSetup = jsonWrite(configSetup, "dns", HTTP.arg("dns"));
  configSetup = jsonWrite(configSetup, "ip", HTTP.arg("ip"));
  configSetup = jsonWrite(configSetup, "checkboxIP", HTTP.arg("checkboxIP"));
  String ssidName = HTTP.arg("ssid");
  String ssidPass = HTTP.arg("ssidPass");
  configSetup = jsonWrite(configSetup, "ssid", ssidName);
  configSetup = jsonWrite(configSetup, "ssidPass", ssidPass);
  HTTP.send(200, "text/plain", "Ok");
  saveConfigSetup();
}

// --------------- Установить имя и пароль для AP
void handle_ssidap() {
  configSetup = jsonWrite(configSetup, "ssidAP", HTTP.arg("ssidAP"));
  configSetup = jsonWrite(configSetup, "ssidApPass", HTTP.arg("ssidApPass"));
  HTTP.send(200, "text/plain", "Ok");
  saveConfigSetup();
}

// -------------- Перезагрузка модуля
void handle_restart() {
  String restart = HTTP.arg("device");
  if (restart == "ok") {                         // Если значение равно Ок
    HTTP.send(200, "text/plain", "Reset OK"); // Oтправляем ответ Reset OK
    ESP.restart();                                // перезагружаем модуль
  } else {                                        // иначе
    HTTP.send(200, "text/plain", "No Reset"); // Oтправляем ответ No Reset
  }
}

//-------------- Перизапустить wifi попытаться узнать будущий ip адрес перезагрузить устройство
bool RestartWiFi() {
  //Перезапуск Wi-Fi при первой настройке
  Serial.println("WiFi reconnect");
    WiFi.mode(WIFI_AP_STA);
  // Не отключаясь от точки доступа подключаемся к роутеру для получения будущего IP
  WiFi.begin(jsonRead(configSetup, "ssid").c_str(),jsonRead(configSetup, "ssidPass").c_str());

  wifiConnect(jsonReadtoInt(configSetup, "wifiConnect"), jsonReadtoInt(configSetup, "wifiBlink"));

  // {"title":"<h3>{{LangConnect2}} <a href="http://192.168.1.30">http://192.168.1.30</a></h3>"}
  // {"title":"Любой текст и html","class":"класс", "style":"Стиль","state":"Данные для вставки в state input_а"}
  String state = "\{\"title\":\"<h3>\{\{LangConnect2\}\} <a href=http://" + WiFi.localIP().toString() + ">http://" + WiFi.localIP().toString() + "</a></h3>\"\}";
  HTTP.send(200, "application/json", state);
  delay(1000);
  // Отключаем точку доступа и переподключаемся к роутеру
  ESP.restart();
}

//-------------Подключение к роутеру конфигурация в строке json
//{"ip":"192.168.0.190","getway":"192.168.0.1","subnet":"255.255.255.0","dns":"","checkboxIP":0}
boolean startSTA(String configWiFi) {
  //WiFi.persistent (false);
  if (jsonRead(configWiFi, "checkboxIP") == "1") {
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
    }
  }
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.hostname ( "sonoff-" + chipID );
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

// ---------------- сканирование сети на доступные точки доступа
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
  }
  String root;
  json.printTo(root);
  HTTP.send(200, "application/json", root);
}


// ----------------- Запускаем WiFi
void startWIFI() {
  if (startSTA(configSetup)) {
  }
  else {
    startAP(configSetup);
  }
}
