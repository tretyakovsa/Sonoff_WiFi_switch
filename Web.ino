// ---------------------Инициализация Web сервера и сервисных функций
void initHTTP(void) {
  // Кэшировать файлы для быстрой работы
  HTTP.serveStatic("/css/", SPIFFS, "/css/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/js/", SPIFFS, "/js/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/img/", SPIFFS, "/img/", "max-age=31536000"); // кеширование на 1 год
  //HTTP.serveStatic("/lang/", SPIFFS, "/lang/", "max-age=31536000"); // кеширование на 1 год
  HTTP.on("/lang", handle_leng);               // Установить язык
  HTTP.on("/config.live.json", handle_ConfigJSON); // Выдаем данные configJson
  HTTP.on("/modules.json", handle_modules);               // Узнать какие модули есть в устройстве
  HTTP.on("/configs", handle_configs);               // Установка конфигурации
  HTTP.on("/cmd", cmd);               // Установка конфигурации
  //Запускаем HTTP сервер
  HTTP.begin();
}

void cmd(){
   command = HTTP.arg("command");
   HTTP.send(200, "text/plain", command);
  }


// --------------------Создаем lang.list.json
void handle_leng_list() {
  HTTP.send(200, "text/plain", Lang);
}

// --------------------Создаем modules.json
void handle_modules() {
  HTTP.send(200, "text/plain", modules);
}


// --------------------Создаем config.live.json
void handle_ConfigJSON() {
  HTTP.send(200, "text/plain", configJson);
}

// ------------------Установка языка
void handle_leng() {
  configJson = jsonWrite(configJson, "lang", HTTP.arg("set"));
  writeFile("config.save.json", configJson );
  HTTP.send(200, "text/plain", "OK");
}

// ------------------Установка конфигурации
void handle_configs() {
  configJson = jsonWrite(configJson, "configs", HTTP.arg("set"));
  writeFile("config.save.json", configJson );
  HTTP.send(200, "text/plain", "OK");
}


