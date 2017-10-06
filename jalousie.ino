// ---------------- Управление двигателем жалюзи
void initJalousie() {
  // Сенсор будет работать по прерыванию
  int pinTurn = readArgsInt();
  attachInterrupt(pinTurn, turn_0, FALLING );
  sendOptions("pinTurn", pinTurn);
  int pinMotor1 = readArgsInt();
  int pinMotor2 = readArgsInt();
  sendOptions("pinMotor1", pinMotor1);
  sendOptions("pinMotor2", pinMotor2);
  pinMode(pinMotor1, OUTPUT);
  pinMode(pinMotor2, OUTPUT);
  digitalWrite(pinMotor1, HIGH);
  digitalWrite(pinMotor2, HIGH);
  sendStatus("stateJalousie", 1);
  sendStatus("turnSensor", 0);
  sendOptions("turn", jsonReadtoInt(configSetup, "turn"));
  sCmd.addCommand("jalousieopen",     jalousieOpen);
  sCmd.addCommand("jalousieclose",    jalousieClose);
  sCmd.addCommand("jalousienot",    jalousieNot);
  sCmd.addCommand("jalousiestop",    jalousieStop);
  sCmd.addCommand("jalousieturn",    jalousieTurn);
  commandsReg("jalousieopen", "jalousie");
  commandsReg("jalousieclose", "jalousie");
  commandsReg("jalousienot", "jalousie");
  //commandsReg("jalousiestop", "jalousie");
  //commandsReg("jalousieturn", "jalousie");
  HTTPWAN.on("/jalousie", jalousieddns);        // реакция на запрос
  HTTPWAN.on("/jalousieopen", jalousieopenddns);        // реакция на запрос
  HTTPWAN.on("/jalousieclose", jalousiecloseddns);        // реакция на запрос
  modulesReg("jalousie");
}

void jalousieddns() {
  jalousieNot();
  HTTPWAN.send(200, "text/plain", "Ok");
}
void jalousieopenddns() {
  jalousieOpen();
  HTTPWAN.send(200, "text/plain", "Ok");
}
void jalousiecloseddns() {
  jalousieClose();
  HTTPWAN.send(200, "text/plain", "Ok");
}
void jalousie() {
  jalousieNot();
  HTTP.send(200, "text/plain", "Ok");
}
void jalousieopen() {
  jalousieOpen();
  HTTP.send(200, "text/plain", "Ok");
}
void jalousieclose() {
  jalousieClose();
  HTTP.send(200, "text/plain", "Ok");
}


// Выполняется при вращение сенсора
void turn_0() {
  static unsigned long millis_prev;
  // Устроняем дребезг контакта
  if (millis() - 500 > millis_prev) {
    //Текущее состояние оборотов
    int turnSensor = jsonReadtoInt(configJson, "turnSensor");
    turnSensor++; // счетчик количества оборотов
    flag = sendStatus("turnSensor", turnSensor);
    int turn = jsonReadtoInt(configSetup, "turn");
    if (turnSensor == turn) {     //Останавливаем
      configJson = jsonWrite(configJson, "turnSensor", 0);
      command = "jalousiestop";

    }
  }
  millis_prev = millis();
}


void jalousieOpen() {
  if (!getStatusInt("stateJalousie")) {
    flag = sendStatus("stateJalousie", 1);
    digitalWrite(getOptionsInt("pinMotor1"), LOW);
    digitalWrite(getOptionsInt("pinMotor2"),  HIGH);
    topicPub("/Jalousie_not/status", String(getStatusInt("stateJalousie")), 1 );
  }
}




void jalousieClose() {
  if (getStatusInt("stateJalousie")) {
    flag = sendStatus("stateJalousie", 0);
    digitalWrite(getOptionsInt("pinMotor1"), HIGH);
    digitalWrite(getOptionsInt("pinMotor2"),  LOW);
    topicPub("/Jalousie_not/status", String(getStatusInt("stateJalousie")), 1 );
  }
}


void jalousieStop() {
  digitalWrite(getOptionsInt("pinMotor1"), HIGH);
  digitalWrite(getOptionsInt("pinMotor2"), HIGH);

}

void jalousieTurn() {
  configSetup = jsonWrite(configSetup, "turn", readArgsInt());
  sendOptions("turn", jsonReadtoInt(configSetup, "turn"));
  saveConfigSetup ();
}

void jalousieNot() {
  if (getStatusInt("stateJalousie")) {
    jalousieClose();
  }
  else {
    jalousieOpen();
  }
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


