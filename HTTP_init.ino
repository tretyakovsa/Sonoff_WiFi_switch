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

// Установка времянной зоны
void handle_time_zone() {
  timeZone = HTTP.arg("timeZone").toInt();
  Time_init(timeZone);
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

//Время из сети
void handle_time() {
  Time_init(timeZone);
  String Time = XmlTime();
  HTTP.send(200, "text/plain", "OK: " + Time);
}

//Таймер 1
void handle_time_1() {
  times1 = HTTP.arg("time1");
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

//Таймер 2
void handle_time_2() {
  times2 = HTTP.arg("time2");
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

void HTTP_init(void) {
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
  HTTPWAN->on("/sonoff", sonoffActiv);                // задать цвет ленты и включить.
  HTTP.on("/reley", releyActiv);                // запуск мотора напровление храниться в переменной
  HTTP.on("/timeSonoff", handle_timesonoff);      // установка времени работы лампы
  HTTP.on("/wifi.scan.json", handle_wifi_scan);      // сканирование ssid
  HTTP.on("/timeZone", handle_time_zone);    // Установка времянной зоны
  HTTP.on("/Time", handle_time);            // Синхронизировать время из сети
  HTTP.on("/times1", handle_time_1);        // Установить время 1
  HTTP.on("/times2", handle_time_2);        // Установить время 2
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
  HTTPWAN->begin();
}

// Получение текущего времени
String XmlTime(void) {
  String Time = ""; // Строка для результатов времени
  int i = 0; // Здесь будем хранить позицию первого символа :
  time_t now = time(nullptr); // получаем времяс помощью библиотеки time.h
  Time += ctime(&now); // Преобразуем время в строку формата
  i = Time.indexOf(":"); //Ишем позицию первого символа :
  Time = Time.substring(i - 2, i + 6); // Выделяем из строки 2 символа перед символом : и 6 символов после
  return Time; // Возврашаем полученное время
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
  json["Temperature"] = "\""+temp+"\"";
  temp = "";
  temp += dht.getHumidity();
  json["Humidity"] = "\""+temp+"\"";
  json["SSDP"] = ssdpName;
  json["ssidAP"] = ssidApName;
  json["ssidApPass"] = ssidApPass;
  json["ssid"] = ssidName;
  json["ssidPass"] = ssidPass;
  json["timeZone"] = timeZone;
  json["timeSonoff"] = timeSonoff; //  Время работы
  json["times1"] = times1; // Время 1
  json["times2"] = times2; // Время 2
  json["ip"] = WiFi.localIP().toString();
  json["time"] = XmlTime(); // Текущее время
  json["lang"] = Language;  // Язык
  json["state"] = state0;
  // Помещаем созданный json в переменную root
  root="";
  json.printTo(root);
  HTTP.send(200, "text/json", root);
}

void handle_ip_list() {
  HTTP.send(200, "text/json", "[{\"ip\":\""+WiFi.localIP().toString()+"\"}"+Devices+"]");
}

void handle_ip_scan() {
  inquirySSDP();
  String json = "";
  //Serial.println(Devices);
  if (Devices != "") {
    json = Devices;
    //json += ",";
  }
  json +=modules();
  DevicesList = "["+json+"]";
  //Serial.println(json);
  HTTP.send(200, "text/json", "[" + json + "]");
  Devices="";
}

void handle_modules() {
 DynamicJsonBuffer jsonBuffer;
 JsonObject& json = jsonBuffer.createObject();
 json["SSDP"] = ssdpName;
 json["state"] = state0;
  JsonArray& data = json.createNestedArray("module");
 for (int i = 0; i < sizeof(module) / sizeof(module[0]); i++) {
    data.add(module[i]);
 }
 String root;
 json.printTo(root);
 HTTP.send(200, "text/json", root);
}

String modules() {
  String json = "";
  for (int i = 0; i <= sizeof(module) / sizeof(module[0]); i++) {
    json += "{\"ip\":\"";
    json += WiFi.localIP().toString();
    json += "\",\"module\":\"";
    json += module[i];
    //Serial.println(module[i]);
    json += "\"";
    json += "}";
    if (i != sizeof(module) / sizeof(module[0])) json += ",";
  }
  return json;
}

void handle_leng_list() {
  HTTP.send(200, "text/json", Lang);
}
