void initUpgrade() {
  // Добавляем функцию Update для перезаписи прошивки по Wi-Fi при 1М(256K SPIFFS) и выше
//  httpUpdater.setup(&HTTP);
  // ----------------------- Обновление с сайта
  HTTP.on("/upgrade", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("Update module...");
    String refresh = "<html><head><meta http-equiv=\"refresh\" content=\"40;/\">Update module...</head></html>";
    request->send(200, "text/html", refresh);
    String spiffsData;
    String spiffs;
    if (request->hasArg("spiffs")) {
      spiffsData = request->arg("spiffs");
      spiffs =spiffsData;
    }
    if (spiffsData != "") {
      //SPIFFS.format();
      spiffsData = spiffsData.substring(spiffsData.lastIndexOf("/") + 1); // выделяем имя файла
      Serial.println(spiffsData);
      ESPhttpUpdate.rebootOnUpdate(false);
      t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs(spiffs);
      switch (ret) {
        case HTTP_UPDATE_FAILED:
          configJson = jsonWrite(configJson, "spiffsData", "HTTP_UPDATE_FAILED");
          statistics();
          break;
        case HTTP_UPDATE_NO_UPDATES:
          configJson = jsonWrite(configJson, "spiffsData", "HTTP_UPDATE_NO_UPDATES");
          statistics();
          break;
        case HTTP_UPDATE_OK:
          configJson = jsonWrite(configJson, "spiffsData", "HTTP_UPDATE_OK");
          statistics();
          break;
      }
      writeFile("timer.save.json", jsonTimer);
      configSetup = jsonWrite(configSetup, "spiffsData", spiffsData);
      saveConfigSetup ();
    }
    String buildData;
    String build;
    if (request->hasArg("build")) {
      buildData = request->arg("build");
      build = buildData;
    }
    if (buildData != "") {
      buildData = buildData.substring(buildData.lastIndexOf("/") + 1); // выделяем имя файла
      Serial.println(buildData);
      configSetup = jsonWrite(configSetup, "buildData", buildData);
      saveConfigSetup ();
      ESPhttpUpdate.rebootOnUpdate(true);
      t_httpUpdate_return jet = ESPhttpUpdate.update(build);
    }
  });

  modulesReg("upgrade");
}

