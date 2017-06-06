void ddns_init() {
  HTTPWAN = ESP8266WebServer (ddnsPort);
  HTTPWAN.on("/sonoff", sonoffActivWan);       // Переключить реле из внешней сети
  HTTP.on("/ddns", handle_ddns);               // Установка параметров ddns
  HTTPWAN.begin();
  modulesReg("ddns");
}

// Установка параметров ddns
void handle_ddns() {
  ddns = HTTP.arg("ddns");
  ddnsName = HTTP.arg("ddnsName");
  ddnsPort = HTTP.arg("ddnsPort").toInt();
  ip_wan();
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

// Меняет флаг для включения выключения sonoff
void sonoffActivWan() {
  command = "relayNot";
  HTTPWAN.send(200, "text/plain", "OK");
}
// Запрос для синхронизации внешнего ip адреса с ddns
int ip_wan() {
  if (ddns!="") {
    HTTPClient http;
    http.begin(ddns); //HTTP

    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == 302) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}



