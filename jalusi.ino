// ---------------- Управление двигателем жалюзи
void initJalousie() {
  // Сенсор будет работать по прерыванию
  int pinTurn = readArgsInt();
  attachInterrupt(pinTurn, turn_0, FALLING );
  sendOptions(pinTurnS, pinTurn);
  int pinMotor1 = readArgsInt();
  int pinMotor2 = readArgsInt();
  sendOptions(pinMotorS + "1", pinMotor1);
  sendOptions(pinMotorS + "2", pinMotor2);
  pinMode(pinMotor1, OUTPUT);
  pinMode(pinMotor2, OUTPUT);
  setMotor(HIGH, HIGH);
  sendStatus(stateJalousieS, 1);
  sendStatus(turnSensorS, 0);
  sendOptions(turnS, jsonReadtoInt(configSetup, turnS));
  // open close not stop turn
  sCmd.addCommand("jalousie",     jalousie);
  commandsReg("jalousie");
  modulesReg("jalousie");
}

// Выполняется при вращение сенсора
void turn_0() {
  static unsigned long millis_prev;
  // Устроняем дребезг контакта
  if (millis() - 500 > millis_prev) {
    //Текущее состояние оборотов
    int turnSensor = jsonReadtoInt(configJson, turnSensorS);
    turnSensor++; // счетчик количества оборотов
    flag = sendStatus(turnSensorS, turnSensor);
    int turn = jsonReadtoInt(configSetup, turnS);
    if (turnSensor == turn) {     //Останавливаем
      jsonWrite(configJson, turnSensorS, 0);
      setMotor(HIGH, HIGH);
    }
  }
  millis_prev = millis();
}


void jalousie() {
  String com = readArgsString();
  if (com == "open") {
    if (!getStatusInt(stateJalousieS)) {
      flag = sendStatus(stateJalousieS, 1);
      setMotor(LOW, HIGH);
    }
  }
  if (com == "close") {
    if (getStatusInt(stateJalousieS)) {
      flag = sendStatus(stateJalousieS, 0);
      setMotor(HIGH, LOW);
    }
  }
  if (com == "stop") {
    setMotor(HIGH, HIGH);
  }
  if (com == "not") {
    if (getStatusInt(stateJalousieS)) {
      flag = sendStatus(stateJalousieS, 0);
      setMotor(HIGH, LOW);
    }
    else {
       flag = sendStatus(stateJalousieS, 1);
      setMotor(LOW, HIGH);
    }
  }
  if (com == "turn") {
    jsonWrite(configSetup, turnS, readArgsInt());
    sendOptions(turnS, jsonReadtoInt(configSetup, turnS));
    saveConfigSetup ();
  }
  statusS = jalousieStatus(configJson, stateJalousieS);
}

void setMotor(boolean a, boolean b) {
  digitalWrite(getOptionsInt(pinMotorS + "1"), a);
  digitalWrite(getOptionsInt(pinMotorS + "2"),  b);
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
