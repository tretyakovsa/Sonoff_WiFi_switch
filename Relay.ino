// ------------------- Инициализация Реле
void initRelay() {
  configLive = jsonWrite(configLive, "relay1Pin", readArgsInt());
  pinMode(jsonReadtoInt(configLive, "relay1Pin"), OUTPUT);
  sCmd.addCommand("relayon",     relayOn);
  sCmd.addCommand("relayoff",    relayOff);
  sCmd.addCommand("relaynot",    relayNot);
  HTTP.on("/relay", relay);        // реакция на запрос
  HTTP.on("/relayon", relayon);        // реакция на запрос
  HTTP.on("/relayoff", relayoff);        // реакция на запрос
  HTTP.on("/sonoff", relay);        // реакция на запрос
  HTTPWAN.on("/relay", relayddns);        // реакция на запрос
  HTTPWAN.on("/relayon", relayonddns);        // реакция на запрос
  HTTPWAN.on("/relayoff", relayoffddns);        // реакция на запрос
  HTTPWAN.on("/sonoff", relayddns);        // реакция на запрос
  modulesReg("relay");
}
void relayddns() {
  sCmd.readStr("relaynot");
  HTTPWAN.send(200, "text/json", relayStatus(configJson, "state"));
}
void relayonddns() {
  sCmd.readStr("relayon");
  HTTPWAN.send(200, "text/json", relayStatus(configJson, "state"));
}
void relayoffddns() {
  sCmd.readStr("relayoff");
  HTTPWAN.send(200, "text/json", relayStatus(configJson, "state"));
}
void relay() {
  sCmd.readStr("relaynot");
  HTTP.send(200, "text/json", relayStatus(configJson, "state"));
}
void relayon() {
  sCmd.readStr("relayon");
  HTTP.send(200, "text/json", relayStatus(configJson, "state"));
}
void relayoff() {
  sCmd.readStr("relayoff");
  HTTP.send(200, "text/json", relayStatus(configJson, "state"));
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

void relayOn() {
  configJson = jsonWrite(configJson, "state", 1);
  int state0 = jsonReadtoInt(configJson, "state");
  toggleRelay(state0);
  digitalWrite(jsonReadtoInt(configLive, "relay1Pin"), state0);
  topicPub("/RELE_1_not/status", String(state0), 1 );
}
void relayOff() {
  configJson = jsonWrite(configJson, "state", 0);
  int state0 = jsonReadtoInt(configJson, "state");
  toggleRelay(state0);
  digitalWrite(jsonReadtoInt(configLive, "relay1Pin"), state0);
  topicPub("/RELE_1_not/status", String(state0), 1 );
}
void relayNot() {
  String str = readArgsString();
  if (str != "") Serial.println(timeToSec(str));


  //configJson = jsonWrite(configJson, "mem", ESP.getFreeHeap());
  configJson = jsonWrite(configJson, "state", !jsonReadtoInt(configJson, "state"));
  int state0 = jsonReadtoInt(configJson, "state");
  toggleRelay(state0);
  digitalWrite(jsonReadtoInt(configLive, "relay1Pin"), state0);
  topicPub("/RELE_1_not/status", String(state0), 1 );
}
void topicPub(String topic, String data, boolean retain ) {
  client.publish(MQTT::Publish(prefix + "/" + chipID + topic,    "{\"status\":" + data + "}").set_retain(1));
}

// -------------- Для управления реле по UART
// https://www.banggood.com/ru/ESP8266-5V-WiFi-Relay-Module-Internet-Of-Things-Smart-Home-Phone-APP-Remote-Control-Switch-p-1126605.html?rmmds=category

void toggleRelay(bool relayState) {
  if (relayState) {
    const byte miBufferON[] = {0xA0, 0x01, 0x01, 0xA2};
    Serial.write(miBufferON, sizeof(miBufferON));
  }
  else {
    //To disable the Relay send it by serial port:
    const byte miBufferOFF[] = {0xA0, 0x01, 0x00, 0xA1};
    Serial.write(miBufferOFF, sizeof(miBufferOFF));
  }
}


