// ------------------- Инициализация  Динамика
void initBuzer() {
  configLive = jsonWrite(configLive, "pinBuzer", readArgsInt());
  pinMode(jsonReadtoInt(configLive, "pinBuzer"), OUTPUT);

  analogWrite(jsonReadtoInt(configLive, "pinBuzer"), readArgsInt());
  analogWriteFreq(0);

}
void buzerBeep() {
  analogWrite(jsonReadtoInt(configLive, "pinBuzer"), readArgsInt());
  analogWriteFreq(readArgsInt());
}


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

// ---------------- Управление двигателем жалюзи
void initJalousie() {
  // Сенсор будет работать по прерыванию
  int pinTurn = readArgsInt();
  attachInterrupt(pinTurn, turn_0, FALLING );
  configLive = jsonWrite(configLive, "pinTurn", pinTurn);
  int pinMotor1 = readArgsInt();
  int pinMotor2 = readArgsInt();
  configLive = jsonWrite(configLive, "pinMotor1", pinMotor1);
  configLive = jsonWrite(configLive, "pinMotor2", pinMotor2);
  configJson = jsonWrite(configJson, "stateJalousie", 1);
  pinMode(pinMotor1, OUTPUT);
  pinMode(pinMotor2, OUTPUT);
  digitalWrite(pinMotor1, HIGH);
  digitalWrite(pinMotor2, HIGH);
  sCmd.addCommand("jalousieopen",     jalousieOpen);
  sCmd.addCommand("jalousieclose",    jalousieClose);
  sCmd.addCommand("jalousienot",    jalousieNot);
  sCmd.addCommand("jalousiestop",    jalousieStop);
  sCmd.addCommand("jalousieturn",    jalousieTurn);
  HTTPWAN.on("/jalousie", jalousieddns);        // реакция на запрос
  HTTPWAN.on("/jalousieopen", jalousieopenddns);        // реакция на запрос
  HTTPWAN.on("/jalousieclose", jalousiecloseddns);        // реакция на запрос
  modulesReg("jalousie");
}

void jalousieddns(){
  jalousieNot();
  HTTPWAN.send(200, "text/plain", "Ok");
  }
  void jalousieopenddns(){
  jalousieOpen();
  HTTPWAN.send(200, "text/plain", "Ok");
  }
  void jalousiecloseddns(){
  jalousieClose();
  HTTPWAN.send(200, "text/plain", "Ok");
  }
void jalousie(){
  jalousieNot();
  HTTP.send(200, "text/plain", "Ok");
  }
  void jalousieopen(){
  jalousieOpen();
  HTTP.send(200, "text/plain", "Ok");
  }
  void jalousieclose(){
  jalousieClose();
  HTTP.send(200, "text/plain", "Ok");
  }


// Выполняется при вращение сенсора
void turn_0() {
  static unsigned long millis_prev;
  // Устроняем дребезг контакта
  if (millis() - 500 > millis_prev) {
    int turnSensor = jsonReadtoInt(configLive, "turnSensor");
    turnSensor++; // счетчик количества оборотов
    configLive = jsonWrite(configLive, "turnSensor", turnSensor);
    int turn = jsonReadtoInt(configJson, "turn");
    if (turnSensor == turn) {     //Останавливаем
      configLive = jsonWrite(configLive, "turnSensor", 0);
      command = "jalousiestop";

    }
  }
  millis_prev = millis();
}

void jalousieOpen() {
  int state0 = jsonReadtoInt(configJson, "stateJalousie");
  if (!state0) {
    configJson = jsonWrite(configJson, "stateJalousie", 1);
    state0 = !state0;
    digitalWrite(jsonReadtoInt(configLive, "pinMotor2"), HIGH);
    digitalWrite(jsonReadtoInt(configLive, "pinMotor1"), LOW);
    topicPub("/jalousie/status", String(state0), 1 );
  }
}
void jalousieClose() {
  int state0 = jsonReadtoInt(configJson, "stateJalousie");
  if (state0) {
    configJson = jsonWrite(configJson, "stateJalousie", 0);
    state0 = !state0;
    digitalWrite(jsonReadtoInt(configLive, "pinMotor1"), HIGH);
    digitalWrite(jsonReadtoInt(configLive, "pinMotor2"), LOW);
    topicPub("/jalousie/status", String(state0), 1 );
  }
}

void jalousieStop() {

  digitalWrite(jsonReadtoInt(configLive, "pinMotor2"), HIGH);
  digitalWrite(jsonReadtoInt(configLive, "pinMotor1"), HIGH);

}

void jalousieTurn() {
  configJson = jsonWrite(configJson, "turn", readArgsInt());
  writeFile("config.save.json", configJson );
}

void jalousieNot() {
  //configJson = jsonWrite(configJson, "mem", ESP.getFreeHeap());
  configJson = jsonWrite(configJson, "stateJalousie", !jsonReadtoInt(configJson, "stateJalousie"));
  int state0 = jsonReadtoInt(configJson, "stateJalousie");
  if (state0) {
    digitalWrite(jsonReadtoInt(configLive, "pinMotor2"), HIGH);
    digitalWrite(jsonReadtoInt(configLive, "pinMotor1"), LOW);
  }
  else {
    digitalWrite(jsonReadtoInt(configLive, "pinMotor1"), HIGH);
    digitalWrite(jsonReadtoInt(configLive, "pinMotor2"), LOW);
  }
  topicPub("/jalousie/status", String(state0), 1 );
}

// читает данные из раздела state строки json и возвращает строку для смены класса кнопки
String jalousieStatus(String json, String state) {
  String out = "{}";
  if (jsonReadtoInt(json, state)) {
    out = jsonWrite(out, "title", "{{LangClose}}");
    out = jsonWrite(out, "class", "btn btn-block btn-lg btn-info");
  }
  else {
    out = jsonWrite(out, "title", "{{LangOpen}}");
    out = jsonWrite(out, "class", "btn btn-block btn-lg btn-primary");

  }
  return out;
}

void initLeakage() {
  int pinMotor1A = readArgsInt();
  int pinMotor2A = readArgsInt();
  int pinMotor3A = readArgsInt();
  int pinMotor4A = readArgsInt();
  int pinMotor1B = readArgsInt();
  int pinMotor2B = readArgsInt();
  int pinMotor3B = readArgsInt();
  int pinMotor4B = readArgsInt();
  configLive = jsonWrite(configLive, "pinMotor1A", pinMotor1A);
  configLive = jsonWrite(configLive, "pinMotor2A", pinMotor2A);
  configLive = jsonWrite(configLive, "pinMotor3A", pinMotor3A);
  configLive = jsonWrite(configLive, "pinMotor4A", pinMotor4A);
  configLive = jsonWrite(configLive, "pinMotor1B", pinMotor1B);
  configLive = jsonWrite(configLive, "pinMotor2B", pinMotor2B);
  configLive = jsonWrite(configLive, "pinMotor3B", pinMotor3B);
  configLive = jsonWrite(configLive, "pinMotor4B", pinMotor4B);
  configJson = jsonWrite(configJson, "stateLeakageA", 1);
  configJson = jsonWrite(configJson, "stateLeakageB", 1);
  initMotor(pinMotor1A, pinMotor2A, pinMotor3A, pinMotor4A);
  initMotor(pinMotor1B, pinMotor2B, pinMotor3B, pinMotor4B);
  //sCmd.addCommand("leakageopen",     leakageOpen);
  //sCmd.addCommand("leakageclose",    leakageClose);
  //sCmd.addCommand("leakagenot",    leakageNot);
  //sCmd.addCommand("leakagestop",    leakageStop);
  modulesReg("leakage");

}

void initMotor(int pinMotor1, int pinMotor2, int pinMotor3, int pinMotor4) {
  pinMode(pinMotor1, OUTPUT);
  pinMode(pinMotor2, OUTPUT);
  pinMode(pinMotor3, OUTPUT);
  pinMode(pinMotor4, OUTPUT);
}

void steperTurn(String motor, int steps, boolean ) {

}

void leakageMotor() {
  configLive = jsonWrite(configLive, "A", 250);
  configLive = jsonWrite(configLive, "B", 250);
  motion.attach(10, motionOff);
  command = jsonRead(configJson, "Command") + "on";

}
void leakageOff() {
  motion.detach();
  command = jsonRead(configJson, "Command") + "off";

}

void stepMotor(int thisStep, String motor) {
  switch (thisStep) {
    case 0:  // 1010
      digitalWrite(jsonReadtoInt(configLive, "pinMotor1" + motor), HIGH);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor2" + motor), LOW);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor3" + motor), HIGH);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor4" + motor), LOW);
      break;
    case 1:  // 0110
      digitalWrite(jsonReadtoInt(configLive, "pinMotor1" + motor), LOW);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor2" + motor), HIGH);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor3" + motor), HIGH);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor4" + motor), LOW);
      break;
    case 2:  //0101
      digitalWrite(jsonReadtoInt(configLive, "pinMotor1" + motor), LOW);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor2" + motor), HIGH);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor3" + motor), LOW);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor4" + motor), HIGH);
      break;
    case 3:  //1001
      digitalWrite(jsonReadtoInt(configLive, "pinMotor1" + motor), HIGH);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor2" + motor), LOW);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor3" + motor), LOW);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor4" + motor), HIGH);
      break;
  }
}
