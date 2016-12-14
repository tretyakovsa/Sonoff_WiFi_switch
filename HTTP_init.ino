void handle_wifiScan() {
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
void handle_Restart() {
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
void handle_saveConfig() {
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

// Время вращения сервопривода
void handle_Timesonoff() {
  Timesonoff = HTTP.arg("t").toInt();
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}
// Установка языка
void handle_SetLeng() {
  Language = HTTP.arg("set");
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

// Установка времянной зоны
void handle_TimeZone() {
  timezone = HTTP.arg("timezone").toInt();
  Time_init(timezone);
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

// Установка параметров сети
void handle_Set_Ssid() {
  _ssid = HTTP.arg("ssid");
  _password = HTTP.arg("password");
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

// Установка параметров сети
void handle_Set_Ssdp() {
  SSDP_Name = HTTP.arg("ssdp");
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

//Установка параметров точки доступа
void handle_Set_Ssidap() {
  _ssidAP = HTTP.arg("ssidAP");
  _passwordAP = HTTP.arg("passwordAP");
  if (HTTP.arg("onOffAP") == "true") {
    _setAP = "1";
  } else {
    _setAP = "0";
  }
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

//Время из сети
void handle_Time() {
  Time_init(timezone);
  String Time = XmlTime();
  HTTP.send(200, "text/plain", "Время синхронизовано: " + Time);
}

//Таймер 1
void handle_Time_1() {
  times1 = HTTP.arg("time1");
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

//Таймер 2
void handle_Time_2() {
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
  HTTP.on("/Timesonoff", handle_Timesonoff);      // установка времени работы лампы
  HTTP.on("/wifiscan.json", handle_wifiScan);      // сканирование ssid
  HTTP.on("/TimeZone", handle_TimeZone);    // Установка времянной зоны
  HTTP.on("/Time", handle_Time);            // Синхронизировать время из сети
  HTTP.on("/times1", handle_Time_1);        // Установить время 1
  HTTP.on("/times2", handle_Time_2);        // Установить время 2
  HTTP.on("/ssdp", handle_Set_Ssdp);        // Установить имя устройства
  HTTP.on("/ssid", handle_Set_Ssid);        // Установить имя и пароль роутера
  HTTP.on("/ssidap", handle_Set_Ssidap);    // Установить имя и пароль для точки доступа
  HTTP.on("/Save", handle_saveConfig);      // Сохранить настройки в файл
  HTTP.on("/configs.json", handle_ConfigXML); // формирование config_xml страницы для передачи данных в web интерфейс
  HTTP.on("/iplocation.xml", handle_IplocationXML);  // формирование iplocation_xml страницы для передачи данных в web интерфейс
  HTTP.on("/restart", handle_Restart);               // Перезагрузка модуля
  HTTP.on("/lang", handle_SetLeng);               // Установить язык
  HTTP.on("/lang.json", handle_Leng);               // Установить язык
  HTTP.on("/ddns", handle_ddns);               // Перезагрузка модуля
  // Запускаем HTTP сервер
  // HTTP.sendHeader("Cache-Control","max-age=2592000, must-revalidate");
  HTTP.on("/devices", inquirySSDP);         // Блок для
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

void handle_ConfigXML() {
  XML = "{";
  // Имя DDNS
  XML += "\"DDNS\":\"";
  XML += DDNS;
  // Температура
  XML += "\",\"Temperature\":\"";
  XML += dht.readTemperature();
  // Влажность
  XML += "\",\"Humidity\":\"";
  XML += dht.readHumidity();
  // Имя SSDP
  XML += "\",\"SSDP\":\"";
  XML += SSDP_Name;
  // Статус AP
  XML += "\",\"onOffAP\":\"";
  XML += _setAP;
  // Имя сети
  XML += "\",\"ssid\":\"";
  XML += _ssid;
  // Пароль сети
  XML += "\",\"password\":\"";
  XML += _password;
  // Имя точки доступа
  XML += "\",\"ssidAP\":\"";
  XML += _ssidAP;
  // Пароль точки доступа
  XML += "\",\"passwordAP\":\"";
  XML += _passwordAP;
  // Времянная зона
  XML += "\",\"timezone\":\"";
  XML += timezone;
  //  Время работы
  XML += "\",\"timesonoff\":\"";
  XML += Timesonoff;
  // Время 1
  XML += "\",\"times1\":\"";
  XML += times1;
  // Время 2
  XML += "\",\"times2\":\"";
  XML += times2;
  // Текущее время
  XML += "\",\"time\":\"";
  XML += XmlTime();
  // Язык
  XML += "\",\"lang\":\"";
  if (Language == NULL) {
    XML += "ru";
  } else {
    XML += Language;
  }
  // Статус
  XML += "\",\"state\":\"";
  XML += state0;
  // IP устройства
  XML += "\",\"ip\":\"";
  XML += WiFi.localIP().toString();
  XML += "\"}";
  HTTP.send(200, "text/json", XML);
}

void handle_IplocationXML() {
  XML = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
  XML += "<List>";
  XML += "<location>";
  XML += "<ip>";
  XML += WiFi.localIP().toString();
  XML += "</ip>";
  XML += Devices;
  XML += "</location>";
  XML += "</List>";
  HTTP.send(200, "text/xml", XML);
}

void handle_Leng() {
  HTTP.send(200, "text/json", Lang);
}
