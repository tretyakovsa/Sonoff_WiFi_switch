// ---------------------Инициализация Web сервера и сервисных функций
void initHTTP(void) {

  HTTP.addHandler(new SPIFFSEditor());
  HTTP.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm");
  HTTP.onNotFound([](AsyncWebServerRequest * request) {
    request->send(404, "text/plain", "FileNotFound");
  });
  // Кэшировать файлы для быстрой работы
  HTTP.serveStatic("/css/", SPIFFS, "/css/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/js/", SPIFFS, "/js/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/img/", SPIFFS, "/img/", "max-age=31536000"); // кеширование на 1 год
  //HTTP.serveStatic("/lang/", SPIFFS, "/lang/", "max-age=31536000"); // кеширование на 1 год

  // --------------------Выдаем данные configOptions
  HTTP.on("/config.options.json", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/json", configOptions);
  });
  // --------------------Выдаем данные configJson
  HTTP.on("/config.live.json", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/json", configJson);
  });
  // -------------------Выдаем данные configSetup
  HTTP.on("/config.setup.json", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/json", configSetup);
  });
  // -------------------Выдаем данные regCommands
  HTTP.on("/command.json", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/json", regCommands);
  });
  // --------------------Узнать какие модули есть в устройстве
  HTTP.on("/modules.json", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/json", modules);
  });
  // -------------------Установка конфигурации
  HTTP.on("/configs", HTTP_GET, [](AsyncWebServerRequest * request) {
    String set;
    if (request->hasArg("set")) {
      set = request->arg("set");
      configSetup = jsonWrite(configSetup, "configs", set);
      configJson = jsonWrite(configJson, "configs", set);
      saveConfigSetup();
    }
    String reqvest = "{\"action\": \"page.htm?configs&" + set + "\"}";
    request->send(200, "text/json", reqvest);
  });
  // ------------------Выполнение команды из запроса
  HTTP.on("/cmd", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasArg("command")) {
      String c = request->arg("command");
      Serial.println(c);
      sCmd.readStr(c);
    }
    request->send(200, "text/json", "Ok");
  });
  // -----------------Параметр языка
  HTTP.on("/lang", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasArg("set")) {
      String set = request->arg("set");
      configSetup = jsonWrite(configSetup, "configs", set);
      configOptions = jsonWrite(configOptions, "configs", set);
      saveConfigSetup();
    }

    request->send(200, "text/plain", "OK");
  });
  //Запускаем HTTP сервер
  HTTP.begin();
}
