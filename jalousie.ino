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


