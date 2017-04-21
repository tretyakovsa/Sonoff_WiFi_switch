void webUpdateSpiffs() {
  Serial.println("Update spiffs...");
  String refresh = "<html><head><meta http-equiv=\"refresh\" content=\"1;http://";
  refresh += WiFi.localIP().toString();
  refresh += "\"></head></html>";
  HTTP.send(200, "text/html", refresh);
  spiffsData = HTTP.arg("upload");
  spiffsData = spiffsData.substring(spiffsData.lastIndexOf("/")+1); // выделяем имя файла
  Serial.println(spiffsData);
  ESPhttpUpdate.rebootOnUpdate(false);
  t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs(HTTP.arg("upload"));
  saveConfig();
}

void webUpdate() {
  Serial.println("Update build...");
  String refresh = "<html><head><meta http-equiv=\"refresh\" content=\"1;http://";
  refresh += WiFi.localIP().toString();
  refresh += "\"></head></html>";
  HTTP.send(200, "text/html", refresh);
  buldData = HTTP.arg("upload");
  buldData = buldData.substring(buldData.lastIndexOf("/")+1); // выделяем имя файла
   Serial.println(buldData);
  saveConfig();
  ESPhttpUpdate.rebootOnUpdate(true);
  t_httpUpdate_return ret = ESPhttpUpdate.update(HTTP.arg("upload"));
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

// Установка языка
void handle_leng() {
  Language = HTTP.arg("set");
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}
// Сохраняет все настройки в файле
void handle_save_config() {
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}



void HTTP_init(void) {
  // Добавляем функцию Update для перезаписи прошивки по Wi-Fi при 1М(256K SPIFFS) и выше
  httpUpdater.setup(&HTTP);
  // Кешировать файлы для быстрой работы
  HTTP.serveStatic("/css/", SPIFFS, "/css/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/js/", SPIFFS, "/js/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/img/", SPIFFS, "/img/", "max-age=31536000"); // кеширование на 1 год
  //HTTP.serveStatic("/lang/", SPIFFS, "/lang/", "max-age=31536000"); // кеширование на 1 год
  HTTP.on("/spiffs", webUpdateSpiffs);                // запустить загрузку fs для обнавления
  HTTP.on("/build", webUpdate);                // запустить загрузку fs для обнавления
  HTTP.on("/save", handle_save_config);      // Сохранить настройки в файл
  HTTP.on("/config.live.json", handle_ConfigJSON); // формирование config_xml страницы для передачи данных в web интерфейс
  HTTP.on("/devices.list.json", handle_ip_list);  // формирование iplocation_xml страницы для передачи данных в web интерфейс
  HTTP.on("/restart", handle_restart);               // Перезагрузка модуля
  HTTP.on("/lang", handle_leng);               // Установить язык
  HTTP.on("/lang.list.json", handle_leng_list);               // Установить язык
  HTTP.on("/modules.json", handle_modules);               // Узнать какие модули есть в устройстве
  // Запускаем HTTP сервер
  HTTP.begin();
}



void handle_ConfigJSON() {
  //{"SSDP":"SSDP-test","ssid":"home","ssidPass":"i12345678","ssidAP":"WiFi","ssidApPass":"","ip":"192.168.0.101"}
  // Резервируем память для json обекта буфер может рости по мере необходимти, предпочтительно для ESP8266
  DynamicJsonBuffer jsonBuffer;
  //  вызовите парсер JSON через экземпляр jsonBuffer
  JsonObject& json = jsonBuffer.parseObject(jsonConfig);
  // Заполняем поля json
  // Заполняем поля json
  json["spiffsData"] = spiffsData;
  json["buldData"] = buldData;
  json["ddns"] = ddns;  // Имя ddns
  json["ddnsName"] = ddnsName;  // Имя ddnsName
  json["ddnsPort"] = ddnsPort;  // Имя ddnsPort
  //String temp = "";
  //temp += dht.getTemperature();
  //json["Temperature"] = temp;
  //temp = "";
  //temp += dht.getHumidity();
  //json["Humidity"] = temp;
  json["SSDP"] = ssdpName;
  json["space"] = spaceName;
  json["timeZone"] = timezone;
  json["timeSonoff"] = timeSonoff; //  Время работы

  json["ip"] = WiFi.localIP().toString();
  json["time"] = GetTime(); // Текущее время
  json["lang"] = Language;  // Язык
  json["pirTime"] = pirTime;  //
  json["state"] = state0;
  // Помещаем созданный json в переменную root
  jsonConfig = "";
  json.printTo(jsonConfig);
  HTTP.send(200, "text/json", jsonConfig);
}

void handle_ip_list() {
  HTTP.send(200, "text/json", "[" + modulesNew + Devices + "]");
}

void handle_modules() {
  HTTP.send(200, "text/json", modulesNew);
}


void handle_leng_list() {
  HTTP.send(200, "text/json", Lang);
}
