void handle_wifi_scan() {
  int n = WiFi.scanNetworks();
  String wifiScan = "[";
  if (n == 0)
    wifiScan = "{\"ssid\":\"none\"}";
  else
  {
    for (int i = 0; i < n - 1; ++i)
    {
      wifiScan += "{";
      wifiScan += "\"ssid\":\"";
      wifiScan += WiFi.SSID(i);
      wifiScan += "\",";
      wifiScan += "\"dbm\":";
      wifiScan += WiFi.RSSI(i);
      wifiScan += ",";
      wifiScan += "\"pass\":\"";
      wifiScan += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "" : "*";
      //wifiScan += WiFi.encryptionType(i);
      wifiScan += "\"}";
      if (i != n - 2) wifiScan += ",";
      delay(10);
    }
    wifiScan += "]";
  }
  HTTP.send(200, "text/json", wifiScan);
}


// Меняет флаг для включения выключения sonoff
void sonoffActiv() {
  chaing = 1;
  HTTP.send(200, "text/plain", "OK");
}

// Установка адреса DDNS сети
void handle_ddns() {
  DDNS = HTTP.arg("url");
  ip_wan();
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

// Перезагрузка модуля
void handle_restart() {
  String restart = HTTP.arg("device");
  if (restart == "ok") {                         // Если значение равно Ок
    HTTP.send(200, "text / plain", "Reset OK"); // Oтправляем ответ Reset OK
    ESP.restart();                                // перезагружаем модуль
  }
  else {                                        // иначе
    HTTP.send(200, "text / plain", "No Reset"); // Oтправляем ответ No Reset
  }
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
  Timesonoff = HTTP.arg("t").toInt();
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
  timezone = HTTP.arg("timezone").toInt();
  Time_init(timezone);
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

// Установка параметров сети
void handle_ssid() {
  _ssid = HTTP.arg("ssid");
  _password = HTTP.arg("password");
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

// Установка параметров сети
void handle_ssdp() {
  SSDP_Name = HTTP.arg("ssdp");
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

//Установка параметров точки доступа
void handle_ssidap() {
  _ssidAP = HTTP.arg("ssidAP");
  _passwordAP = HTTP.arg("passwordAP");
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

//Время из сети
void handle_time() {
  Time_init(timezone);
  String Time = XmlTime();
  HTTP.send(200, "text/plain", "Время синхронизовано: " + Time);
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
  HTTP.on("/reley", releyActiv);                // запуск мотора напровление храниться в переменной
  HTTP.on("/Timesonoff", handle_timesonoff);      // установка времени работы лампы
  HTTP.on("/wifi.scan.json", handle_wifi_scan);      // сканирование ssid
  HTTP.on("/TimeZone", handle_time_zone);    // Установка времянной зоны
  HTTP.on("/Time", handle_time);            // Синхронизировать время из сети
  HTTP.on("/times1", handle_time_1);        // Установить время 1
  HTTP.on("/times2", handle_time_2);        // Установить время 2
  HTTP.on("/ssdp", handle_ssdp);        // Установить имя устройства
  HTTP.on("/ssid", handle_ssid);        // Установить имя и пароль роутера
  HTTP.on("/ssidap", handle_ssidap);    // Установить имя и пароль для точки доступа
  HTTP.on("/Save", handle_save_config);      // Сохранить настройки в файл
  HTTP.on("/configs.json", handle_config); // формирование config_xml страницы для передачи данных в web интерфейс
  HTTP.on("/devices.scan.json", inquirySSDP);  // формирование iplocation_xml страницы для передачи данных в web интерфейс
  HTTP.on("/devices.list.json", handle_ip_list);  // формирование iplocation_xml страницы для передачи данных в web интерфейс
  HTTP.on("/restart", handle_restart);               // Перезагрузка модуля
  HTTP.on("/lang", handle_leng);               // Установить язык
  HTTP.on("/lang.list.json", handle_leng_list);               // Установить язык
  HTTP.on("/ddns", handle_ddns);               // Перезагрузка модуля
  HTTP.on("/modules.json", handle_modules);               // Узнать какие модули есть в устройстве
  HTTP.on("/sensor.json", handle_sensor);               //
  // Запускаем HTTP сервер
  HTTP.begin();
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

void handle_config() {
  String json = "{";
  // Имя DDNS
  json += "\"DDNS\":\"";
  json += DDNS;
  // Температура
  json += "\",\"Temperature\":\"";
  json += dht.readTemperature();
  // Влажность
  json += "\",\"Humidity\":\"";
  json += dht.readHumidity();
  // Имя SSDP
  json += "\",\"SSDP\":\"";
  json += SSDP_Name;
  // Имя сети
  json += "\",\"ssid\":\"";
  json += _ssid;
  // Пароль сети
  json += "\",\"password\":\"";
  json += _password;
  // Имя точки доступа
  json += "\",\"ssidAP\":\"";
  json += _ssidAP;
  // Пароль точки доступа
  json += "\",\"passwordAP\":\"";
  json += _passwordAP;
  // Времянная зона
  json += "\",\"timezone\":\"";
  json += timezone;
  //  Время работы
  json += "\",\"timesonoff\":\"";
  json += Timesonoff;
  // Время 1
  json += "\",\"times1\":\"";
  json += times1;
  // Время 2
  json += "\",\"times2\":\"";
  json += times2;
  // Текущее время
  json += "\",\"time\":\"";
  json += XmlTime();
  // Язык
  json += "\",\"lang\":\"";
  if (Language == NULL) {
    json += "ru";
  } else {
    json += Language;
  }
  // Статус
  json += "\",\"state\":\"";
  json += state0;
  json += "\"}";
  HTTP.send(200, "text/json", json);
}

void handle_sensor() {
  String json = "\"Temperature\":";
  json += dht.readTemperature();
  // Влажность
  json += ",\"Humidity\":";
  json += dht.readHumidity();
  HTTP.send(200, "text/json", "[{"+json+"}]");
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
  HTTP.send(200, "text/json", modules());
}

String modules() {
  String json = "";
  int j = a - 1;
  for (int i = 0; i <= j; i++) {
    json += "{\"ip\":\"";
    json += WiFi.localIP().toString();
    json += "\",\"module\":\"";
    json += module[i];
    //Serial.println(module[i]);
    json += "\"";
    json += "}";
    if (i != j) json += ",";
  }
  return json;
}

void handle_leng_list() {
  HTTP.send(200, "text/json", Lang);
}
