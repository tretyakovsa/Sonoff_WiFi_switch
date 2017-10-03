// ------------------- Инициализация Реле
void initRelay() {
  int pin = readArgsInt();
  sendOptions("relay1Pin", pin);
  pinMode(pin, OUTPUT);
  sendStatus("state", readArgsInt());
  digitalWrite(pin,   getStatusInt("state"));
    // --------------------реакция на запрос
  HTTP.on("/relay", HTTP_GET, [](AsyncWebServerRequest * request) {
    sCmd.readStr("relaynot");
    request->send(200, "application/json", relayStatus(configJson, "state"));
  });
  sCmd.addCommand("relaynot",    relayNot);
  commandsReg("relayon", "relay");
  commandsReg("relayoff", "relay");
  commandsReg("relaynot","relay");
  commandsReg("print","relay");
  modulesReg("relay");
  modulesReg("relay1");
}
void relayNot() {
  flag = sendStatus("state", !getStatusInt("state"));
  digitalWrite(getOptionsInt("relay1Pin"),   getStatusInt("state"));
}
// читает данные из раздела state строки json и возвращает строку для смены класса кнопки
String relayStatus(String json, String state) {
  String out = "{}";
  if (jsonReadtoInt(json, state)) {
    out = jsonWrite(out, "title", "{{LangOff}}");
    out = jsonWrite(out, "class", "btn btn-block btn-lg btn-info");
  }
  else {
    out = jsonWrite(out, "title", "{{LangOn}}");
    out = jsonWrite(out, "class", "btn btn-block btn-lg btn-primary");

  }
  return out;
}


// ---------- initD18B20
void initD18B20() {
  pinMode(d18b20PIN, INPUT_PULLUP);
  d18b20.begin();
  d18b20.setResolution(12);
  d18b20.requestTemperatures();
  float ok = d18b20.getTempCByIndex(0);
    // задача проверять таймеры каждую секунду.
  ts.add(3, 1000, [&](void*) {
    int temp = analogRead(A0);
    if (temp>512) flag = sendStatus("temperature", analogRead(A0));

  }, nullptr, true);


  HTTP.on("/temperature.json", HTTP_GET, [](AsyncWebServerRequest * request) {
    int temp = jsonRead(configSetup, "temperature").toInt();
    request->send(200, "text/json", graf(temp, 10, 3000));
  });

  modulesReg("temperature");

}



