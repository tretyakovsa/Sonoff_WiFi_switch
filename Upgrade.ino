void initUpgrade() {
  // Добавляем функцию Update для перезаписи прошивки по Wi-Fi при 1М(256K SPIFFS) и выше
  httpUpdater.setup(&HTTP);
  HTTP.on("/upgrade", webUpgrade);                // запустить обновление
  modulesReg("upgrade");
}
// ----------------------- Обновление с сайта
void webUpgrade() {
  String refresh = "<html><head><link rel=\"stylesheet\" type=\"text/css\" href=\"./css/build.css\"><meta http-equiv=\"refresh\" content=\"60; url=/\"></head><body><br><br><center><div class=\"loader\"></div><h1>Update module... <br><span id=\"countdownt\">60 </span> seconds...</h1></center>   <script>var timeleft=60;var downloadTimer=setInterval(function(){timeleft--;document.getElementById(\"countdownt\").textContent=timeleft;if(timeleft <= 0)clearInterval(downloadTimer);},1000);</script></body></html>";
  httpOkHtml(refresh);
    //httpOkJson(refresh);
  String Timerset = readFile(configTimerS, 4096);
  String spiffsData = HTTP.arg(spiffsS);
  if (spiffsData != emptyS) {
    spiffsData = spiffsData.substring(spiffsData.lastIndexOf("/") + 1); // выделяем имя файла
    ESPhttpUpdate.rebootOnUpdate(false);
    //Serial.println(buildPach);
    t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs(HTTP.arg(spiffsS));
switch(ret) {
            case HTTP_UPDATE_FAILED:
            sendSetup(spiffsDataS, "Spiffs_FAILED");
            statistics();
            break;
            case HTTP_UPDATE_NO_UPDATES:
            sendSetup(spiffsDataS, "Spiffs_NO_UPDATES");
            statistics();
                break;
            case HTTP_UPDATE_OK:
            sendSetup(spiffsDataS, "Spiffs_UPDATE_OK");
            statistics();
                break;
        }
     writeFile(configTimerS, Timerset);
  if (Scenary !=""){
    writeFile(ScenaryS, Scenary);
    }
    sendSetup(spiffsDataS, spiffsData);
    saveConfigSetup ();
  }
  String buildData = HTTP.arg("build");
  if (buildData != emptyS) {
    writeFile("buildPach.txt", buildData);
    buildData = buildData.substring(buildData.lastIndexOf("/") + 1); // выделяем имя файла
    sendSetup(buildDataS, buildData);
    saveConfigSetup ();
    ESPhttpUpdate.rebootOnUpdate(true);
    String updatePatch = "http://backup.privet.lv/esp/sonoff/Sonoff_Updater.ino.generic.bin";
    //Serial.println(updatePatch);
    t_httpUpdate_return jet = ESPhttpUpdate.update(updatePatch);

  }
}


