void handle_wifi_scan() {
  int n = WiFi.scanNetworks();
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  JsonArray& networks = json.createNestedArray("networks");
  for(int i=0;i<n;i++) {
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

void webUpdateSpiffs() {
  String refresh = "<html><head><meta http-equiv=\"refresh\" content=\"1;http://";
  refresh += WiFi.localIP().toString();
  refresh += "\"></head></html>";
  HTTP.send(200, "text/html", refresh);
  //t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs("http://backup.privet.lv/rgb_spiffs_1m_256k.bin");
  saveConfig();
}

// Перезагрузка модуля
void handle_restart() {
  String restart = HTTP.arg("device");
  if (restart == "ok") {                         // Если значение равно Ок
    HTTP.send(200, "text/plain", "Reset OK"); // Oтправляем ответ Reset OK
    ESP.restart();                                // перезагружаем модуль
  } else {                                        // иначе
    HTTP.send(200, "text/plain", "No Reset"); // Oтправляем ответ No Reset
  }
}

// Меняет флаг для включения выключения sonoff
void sonoffActiv() {
  chaing = 1;
  HTTP.send(200, "text/plain", "OK");
}

// Меняет флаг для включения выключения sonoff
void sonoffActivWan() {
  chaing = 1;
  HTTPWAN.send(200, "text/plain", "OK");
}

// Установка параметров сети
void handle_ddns() {
  ddns = HTTP.arg("ddns");
  ddnsName = HTTP.arg("ddnsName");
  ddnsPort = HTTP.arg("ddnsPort").toInt();
  //Serial.println(HTTP.arg("url"));
  //Serial.println(HTTP.arg("ddnsName"));
  ip_wan();
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

// Меняет флаг для запуска
void releyActiv() {
  Serial.println(state0);
  chaing = 1;
  HTTP.send(200, "text/plain", "OK");
}

// Сохраняет все настройки в файле
void handle_save_config() {
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

// Время вращения сервопривода
void handle_timesonoff() {
  timeSonoff = HTTP.arg("t").toInt();
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}
// Установка языка
void handle_leng() {
  Language = HTTP.arg("set");
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

// Установка параметров сети
void handle_ssid() {
  ssidName = HTTP.arg("ssid");
  ssidPass = HTTP.arg("ssidPass");
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

// Установка параметров сети
void handle_ssdp() {
  ssdpName = HTTP.arg("ssdp");
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

//Установка параметров точки доступа
void handle_ssidap() {
  ssidApName = HTTP.arg("ssidAP");
  ssidApPass = HTTP.arg("ssidApPass");
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}


// Pir таймер
void handle_pir() {
  pirTime = HTTP.arg("t").toInt();
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

void HTTP_init(void) {
  HTTPWAN = ESP8266WebServer (ddnsPort);
  // SSDP дескриптор
  HTTP.on("/description.xml", HTTP_GET, []() {
    SSDP.schema(HTTP.client());
  });
  // Добавляем функцию Update для перезаписи прошивки по Wi-Fi при 1М(256K SPIFFS) и выше
  httpUpdater.setup(&HTTP);
  HTTP.on("/restartWiFi", RestartWiFi);                // задать цвет ленты и включить.
  // HTTP.on("/webupdatespiffs", webUpdateSpiffs);                // задать цвет ленты и включить.
  HTTP.serveStatic("/css/", SPIFFS, "/css/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/js/", SPIFFS, "/js/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/img/", SPIFFS, "/img/", "max-age=31536000"); // кеширование на 1 год
  //HTTP.serveStatic("/lang/", SPIFFS, "/lang/", "max-age=31536000"); // кеширование на 1 год
  HTTP.on("/sonoff", sonoffActiv);                // задать цвет ленты и включить.
  HTTPWAN.on("/sonoff", sonoffActivWan);                // задать цвет ленты и включить.
  HTTP.on("/reley", releyActiv);                // запуск мотора напровление храниться в переменной
  HTTP.on("/timeSonoff", handle_timesonoff);      // установка времени работы лампы
  HTTP.on("/wifi.scan.json", handle_wifi_scan);      // сканирование ssid
  HTTP.on("/pir", handle_pir);        // Устанавливаем время работы pir сенсра
  HTTP.on("/ssdp", handle_ssdp);        // Установить имя устройства
  HTTP.on("/ssid", handle_ssid);        // Установить имя и пароль роутера
  HTTP.on("/ssidap", handle_ssidap);    // Установить имя и пароль для точки доступа
  HTTP.on("/save", handle_save_config);      // Сохранить настройки в файл
  HTTP.on("/config.live.json", handle_ConfigJSON); // формирование config_xml страницы для передачи данных в web интерфейс
  HTTP.on("/devices.scan.json", inquirySSDP);  // формирование iplocation_xml страницы для передачи данных в web интерфейс
  HTTP.on("/devices.list.json", handle_ip_list);  // формирование iplocation_xml страницы для передачи данных в web интерфейс
  HTTP.on("/restart", handle_restart);               // Перезагрузка модуля
  HTTP.on("/lang", handle_leng);               // Установить язык
  HTTP.on("/lang.list.json", handle_leng_list);               // Установить язык
  HTTP.on("/ddns", handle_ddns);               // Перезагрузка модуля
  HTTP.on("/modules.json", handle_modules);               // Узнать какие модули есть в устройстве
  // Запускаем HTTP сервер
  HTTP.begin();
  HTTPWAN.begin();
}



void handle_ConfigJSON() {
  String root = "{}";  // Формировать строку для отправки в браузер json формат
  //{"SSDP":"SSDP-test","ssid":"home","ssidPass":"i12345678","ssidAP":"WiFi","ssidApPass":"","ip":"192.168.0.101"}
  // Резервируем память для json обекта буфер может рости по мере необходимти, предпочтительно для ESP8266
  DynamicJsonBuffer jsonBuffer;
  //  вызовите парсер JSON через экземпляр jsonBuffer
  JsonObject& json = jsonBuffer.parseObject(root);
  // Заполняем поля json
  // Заполняем поля json
  json["ddns"] = ddns;  // Имя ddns
  json["ddnsName"] = ddnsName;  // Имя ddnsName
  json["ddnsPort"] = ddnsPort;  // Имя ddnsPort
  String temp = "";
  temp += dht.getTemperature();
  json["Temperature"] = temp;
  temp = "";
  temp += dht.getHumidity();
  json["Humidity"] = temp;
  json["SSDP"] = ssdpName;
  json["ssidAP"] = ssidApName;
  json["ssidApPass"] = ssidApPass;
  json["ssid"] = ssidName;
  json["ssidPass"] = ssidPass;
  json["timeZone"] = timezone;
  json["timeSonoff"] = timeSonoff; //  Время работы

  json["ip"] = WiFi.localIP().toString();
  json["time"] = GetTime(); // Текущее время
  json["lang"] = Language;  // Язык
  json["pirTime"] = pirTime;  //
  json["state"] = state0;
  // Помещаем созданный json в переменную root
  root="";
  json.printTo(root);
  HTTP.send(200, "text/json", root);
}

void handle_ip_list() {
  HTTP.send(200, "text/json", "["+modules()+Devices+"]");
}

void handle_modules() {
  HTTP.send(200, "text/json", modules());
}

String modules() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["ip"] = WiFi.localIP().toString();
  json["SSDP"] = ssdpName;
  JsonArray& data = json.createNestedArray("module");
  for (int i = 0; i < sizeof(module) / sizeof(module[0]); i++) {
    data.add(module[i]);
  }
  String root;
  json.printTo(root);
  return root;
}

void handle_leng_list() {
  HTTP.send(200, "text/json", Lang);
}
