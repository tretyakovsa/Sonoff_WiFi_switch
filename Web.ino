// ---------------------Инициализация Web сервера и сервисных функций
void initHTTP(void) {
  // Кэшировать файлы для быстрой работы
  HTTP.serveStatic("/css/", SPIFFS, "/css/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/js/", SPIFFS, "/js/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/img/", SPIFFS, "/img/", "max-age=31536000"); // кеширование на 1 год
  //HTTP.serveStatic("/lang/", SPIFFS, "/lang/", "max-age=31536000"); // кеширование на 1 год
  sendStatus("voice", "go");
  // --------------------Выдаем данные configOptions
  HTTP.on("/config.options.json", HTTP_GET, []() {
    httpOkJson(configOptions);
  });

    // --------------------Выдаем данные configJson
  HTTP.on("/config.live.json", HTTP_GET, []() {
    httpOkJson(configJson);
  });

  // -------------------Выдаем данные configSetup
  HTTP.on("/config.setup.json", HTTP_GET, []() {
    httpOkJson(configSetup);
  });
  // -------------------Выдаем данные regCommands
  HTTP.on("/command.json", HTTP_GET, []() {
    httpOkJson(regCommands);
  });

  // --------------------Узнать какие модули есть в устройстве
  HTTP.on("/modules.json", HTTP_GET, []() {
    httpOkJson(modules);
  });

  // -------------------Установка конфигурации
  HTTP.on("/configs", HTTP_GET, []() {
    String set = HTTP.arg("set");
    jsonWrite(configSetup, configsS, set);
    saveConfigSetup();
    String reqvest = "{\"action\": \"page.htm?configs&" + set + "\"}";
    httpOkText(reqvest);
  });

  // ------------------Выполнение команды из запроса
  HTTP.on("/cmd", HTTP_GET, []() {
    String com = HTTP.arg("command");
    //Serial.println(com);
    sendStatus("test", com);
    sCmd.readStr(com);
    httpOkText(statusS);
    //Serial.println(statusS);
  });
  // ------------------Выполнение голосовой команды
    HTTP.on("/dusi", HTTP_GET, []() {
    String com = HTTP.arg("command");
    com.replace(" ", "_");
    //com.toLowerCase();
    //Serial.println(com);
    flag = sendStatus("voice", com);
    //handleScenary();
    //sCmd.readStr(com);
    httpOkText(statusS);
    //Serial.println(statusS);
  });

  HTTP.on("/lang", HTTP_GET, []() {
    jsonWrite(configSetup, langS, HTTP.arg("set"));
    sendOptions(langS, jsonRead(configSetup, langS));
    jsonWrite(modules, langS, jsonRead(configSetup, langS));
    saveConfigSetup();
    httpOkText();
  });

  //Запускаем HTTP сервер
  HTTP.begin();
}
