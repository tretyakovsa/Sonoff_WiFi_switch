// ------------------- Инициализация Реле
void initRelay() {
  int pin = readArgsInt();
  sendOptions("relay1Pin", pin);
  pinMode(pin, OUTPUT);
  sendStatus("state", readArgsInt());
  digitalWrite(pin,   getStatusInt("state"));

  sCmd.addCommand("relayon",     relayOn);
  sCmd.addCommand("relayoff",    relayOff);
  sCmd.addCommand("relaynot",    relayNot);
  commandsReg("relayon", "relay");
  commandsReg("relayoff", "relay");
  commandsReg("relaynot", "relay");

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
  HTTPWAN.send(200, "application/json", relayStatus(configJson, "state"));
}
void relayonddns() {
  sCmd.readStr("relayon");
  HTTPWAN.send(200, "application/json", relayStatus(configJson, "state"));
}
void relayoffddns() {
  sCmd.readStr("relayoff");
  HTTPWAN.send(200, "application/json", relayStatus(configJson, "state"));
}
void relay() {
  sCmd.readStr("relaynot");
  HTTP.send(200, "application/json", relayStatus(configJson, "state"));
}
void relayon() {
  sCmd.readStr("relayon");
  HTTP.send(200, "application/json", relayStatus(configJson, "state"));
}
void relayoff() {
  sCmd.readStr("relayoff");
  HTTP.send(200, "application/json", relayStatus(configJson, "state"));
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
  Serial.println(getStatusInt("state"));
  if (!getStatusInt("state")) flag = sendStatus("state", 1);
  digitalWrite(getOptionsInt("relay1Pin"),   getStatusInt("state"));
  toggleRelay(getStatusInt("state"));
  topicPub("/RELE_1_not/status", String(getStatusInt("state")), 1 );
}

void relayOff() {
  Serial.println(getStatusInt("state"));
  if (getStatusInt("state")) flag = sendStatus("state", 0);
  digitalWrite(getOptionsInt("relay1Pin"),   getStatusInt("state"));
  toggleRelay(getStatusInt("state"));
  topicPub("/RELE_1_not/status", String(getStatusInt("state")), 1 );
}

void relayNot() {
  flag = sendStatus("state", !getStatusInt("state"));
  digitalWrite(getOptionsInt("relay1Pin"),   getStatusInt("state"));
  toggleRelay(getStatusInt("state"));
  topicPub("/RELE_1_not/status", String(getStatusInt("state")), 1 );
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


