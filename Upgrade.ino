void initUpgrade() {
    // Добавляем функцию Update для перезаписи прошивки по Wi-Fi при 1М(256K SPIFFS) и выше
  httpUpdater.setup(&HTTP);
    HTTP.on("/upgrade", webUpgrade);                // запустить обнавление
    modulesReg("upgrade");
  }
// ----------------------- Обновление с сайта
void webUpgrade() {
  Serial.println("Update module...");
  String refresh = "<html><head><meta http-equiv=\"refresh\" content=\"40;/\">Update module...</head></html>";
  HTTP.send(200, "text/html", refresh);
  String spiffsData = HTTP.arg("spiffs");
  if (spiffsData != "") {
    spiffsData = spiffsData.substring(spiffsData.lastIndexOf("/") + 1); // выделяем имя файла
    Serial.println(spiffsData);
    //String modules = readFile("config.modules.json", 4096);
    ESPhttpUpdate.rebootOnUpdate(false);
    t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs(HTTP.arg("spiffs"));
    //writeFile("config.modules.json", modules);
    writeFile("timer.save.json", jsonTimer);
    //timer_Save();
    configJson = jsonWrite(configJson, "spiffsData", spiffsData);
    writeFile("config.save.json", configJson );
  }
  String buildData = HTTP.arg("build");
  if (buildData != "") {
    buildData = buildData.substring(buildData.lastIndexOf("/") + 1); // выделяем имя файла
    Serial.println(buildData);
    configJson = jsonWrite(configJson, "buildData", buildData);
    writeFile("config.save.json", configJson );
    ESPhttpUpdate.rebootOnUpdate(true);
    t_httpUpdate_return jet = ESPhttpUpdate.update(HTTP.arg("build"));
  }
}
