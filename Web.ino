// ---------------------Инициализация Web сервера и сервисных функций
void initHTTP(void) {
  // Кэшировать файлы для быстрой работы
  HTTP.serveStatic("/css/", SPIFFS, "/css/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/js/", SPIFFS, "/js/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/img/", SPIFFS, "/img/", "max-age=31536000"); // кеширование на 1 год
  //HTTP.serveStatic("/lang/", SPIFFS, "/lang/", "max-age=31536000"); // кеширование на 1 год

  // --------------------Выдаем данные configOptions
  HTTP.on("/config.options.json", HTTP_GET, []() {
    HTTP.send(200, "application/json", configOptions);
  });

    // --------------------Выдаем данные configJson
  HTTP.on("/config.live.json", HTTP_GET, []() {
    HTTP.send(200, "application/json", configJson);
  });

  // -------------------Выдаем данные configSetup
  HTTP.on("/config.setup.json", HTTP_GET, []() {
    HTTP.send(200, "application/json", configSetup);
  });
  // -------------------Выдаем данные regCommands
  HTTP.on("/command.json", HTTP_GET, []() {
    HTTP.send(200, "application/json", regCommands);
  });

  // --------------------Узнать какие модули есть в устройстве
  HTTP.on("/modules.json", HTTP_GET, []() {
    HTTP.send(200, "application/json", modules);
  });

  // -------------------Установка конфигурации
  HTTP.on("/configs", HTTP_GET, []() {
    String set = HTTP.arg("set");
    configSetup = jsonWrite(configSetup, "configs", set);
    saveConfigSetup();
    String reqvest = "{\"action\": \"page.htm?configs&" + set + "\"}";
    HTTP.send(200, "text/plain", reqvest);
  });

  // ------------------Выполнение команды из запроса
  HTTP.on("/cmd", HTTP_GET, []() {
    command = HTTP.arg("command");
    HTTP.send(200, "text/plain", command);
  });

  HTTP.on("/lang", HTTP_GET, []() {
    configSetup = jsonWrite(configSetup, "lang", HTTP.arg("set"));
    configOptions = jsonWrite(configOptions, "lang", HTTP.arg("set"));
    saveConfigSetup();
    HTTP.send(200, "text/plain", "OK");
  });

  //Запускаем HTTP сервер
  HTTP.begin();
}
