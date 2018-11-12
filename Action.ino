// ------------------- Инициализация Реле
void initRelay() {
  uint8_t pin = readArgsInt(); // первый аргумент pin
  pin =  pinTest(pin);
  String num = readArgsString(); // второй аргумент прификс реле 0 1 2
  boolean state = readArgsInt(); // третий  аргумент состояние на старте
  boolean inv = readArgsInt(); // четвертый аргумент инверсия выхода
  String title = readArgsString(); // Пятый аргумент подпись
  String nameR = relayS + num;
  if (title=="" ) title = nameR;
  sendStatus(nameR, state);
  sCmd.readStr("wReg toggle "+nameR+" "+title);
  sendOptions(relayPinS + num, pin);
  sendOptions(relayNotS + num, inv);
  // 19 pin это реле через UART
  if (pin > 19 ) {
    Serial.begin(9600);
    delay(100);
    relayWrite(pin, state ^ inv);
  }
  else {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, state ^ inv);
  }
  sCmd.addCommand(relayS.c_str(),relay); //
  commandsReg(relayS);
  modulesReg(relayS + num);
}

// http://192.168.0.91/cmd?command=relay off 1
void relay() {
  String com = readArgsString(); // действие
  String num = readArgsString(); // номер реле
  uint32_t times = readArgsInt(); // время
  String del = readArgsString(); // это delay
  relaySet(num, com);
  if (times != 0) {
    if (del == "t") {
      impulsTime(times - 1, relayS + " not " + num);
    } else {
      comTimeP = relayS + " not " + num;
      String t = GetTime();
      pTime = timeToString(timeToLong(t) + (times - 1));
    }
  } else {
    comTimeP = emptyS;
    pTime = emptyS;
  }
}

void relaySet(String num, String com) {
  String kayPin = relayPinS + num; // Получим имя ячейки пин по номеру
  String kay = relayS + num; // Имя реле
  uint8_t pin = getOptionsInt(kayPin); // Получим пин по Имени реле
  uint8_t inv = getOptionsInt(relayNotS + num); // Получим признак инверсии по Имени реле
  uint8_t state = getStatusInt(kay); // Получим статус реле по Имени
  // Проверим команду приготовим новый state
  if (com == onS || com == "1" ) state = 1;
  if (com == offS || com == "0") state = 0;
  if (com == notS) state = !(state);
  if (pin  > 19) {
    relayWrite(pin, state ^ inv);
  } else {
    digitalWrite(pin, state ^ inv);
  }
  flag = sendStatus(kay, state);
  //sendMQTTstatus(kay, "status", state);
  statusS = htmlStatus(configJson, kay, langOnS, langOffS);
}

// -------------- Для управления реле по UART
// https://www.banggood.com/ru/ESP8266-5V-WiFi-Relay-Module-Internet-Of-Things-Smart-Home-Phone-APP-Remote-Control-Switch-p-1126605.html?rmmds=category

void relayWrite(uint8_t vpin, boolean state) {
  if (state) {
    const byte miBufferON[] = {0xA0, 0x01, 0x01, 0xA2};
    Serial.write(miBufferON, sizeof(miBufferON));
  }
  else {
    const byte miBufferOFF[] = {0xA0, 0x01, 0x00, 0xA1};
    Serial.write(miBufferOFF, sizeof(miBufferOFF));
  }
}

String htmlStatus(String json, String state, String sOn, String sOff) {
  String out = "{}";
  if (jsonReadToInt(json, state)) {
    jsonWrite(out, titleS, sOff);
    jsonWrite(out, classS, btnS + infoS);
  }
  else {
    jsonWrite(out, titleS, sOn);
    jsonWrite(out, classS, btnS + primaryS);
  }
  return out;
}

/*
  XdrvSetPower(rpower);

  if ((SONOFF_DUAL == Settings.module) || (CH4 == Settings.module)) {
    Serial.write(0xA0);
    Serial.write(0x04);
    Serial.write(rpower &0xFF);
    Serial.write(0xA1);
    Serial.write('\n');
    Serial.flush();
  }
   где вместо "0xxx" - 0 (выключить оба реле), 1 (включить одно реле), 2 (включить второе реле), 3 (включить оба реле), то можно таким образом управлять релюшками.
*/
// ------------------- Инициализация Buzera
void initBuzzer() {
  uint8_t pin = readArgsInt(); // первый аргумент pin
  pin =  pinTest(pin);
  sendOptions(buzzerPinS, pin);
  sCmd.addCommand(toneS.c_str(), buzzerTone);
  commandsReg(toneS);
  modulesReg(toneS);
}

void buzzerTone() {
  int freq = readArgsInt();
  int duration = readArgsInt();
  uint8_t pin = getOptionsInt(buzzerPinS);
  tone(pin, freq, duration);
}
// ----------------------Передатчик на 433мГ
void rfTransmitter() {
  byte pin = readArgsInt();
  pin =  pinTest(pin, HIGH);
  mySwitch.enableTransmit(pin);
  sCmd.addCommand("rfsend", handleRfTransmit);
  commandsReg("rfsend");
  modulesReg("rfTransmitter");
}

void handleRfTransmit() {
  int cod = readArgsInt();
  int len = readArgsInt();
  if (len == 0) len = 24;
  mySwitch.send(cod, len);
}

// ----------------------Передатчик Livolvo на 433мГ
void rfLivolo() {
  byte pin = readArgsInt();
  pin =  pinTest(pin, HIGH);
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  gLivolo = new LivoloTx(pin);
  sCmd.addCommand("lvsend", handleRfLivolo);
  commandsReg("lvsend");
  modulesReg("rfLivolo");
}

void handleRfLivolo() {
  int cod = readArgsInt();
  int len = readArgsInt();
  gLivolo->sendButton(cod, len);
}

#ifdef CRIB
// ------------------- Инициализация кроватка
void initCrib() {
  uint8_t pin = readArgsInt(); // первый аргумент pin
  pin =  pinTest(pin);
  pinMode(pin, OUTPUT);
  sendOptions(cribPinS, pin);
  sCmd.addCommand(cribS.c_str(), startCrib);
  commandsReg(cribS);
  modulesReg(cribS);
}

void startCrib() {
  int freq = readArgsInt(); // Как долго включен
  int freq1 = readArgsInt(); // Как долго выключен
  int duration = readArgsInt(); // Количество повторов
  uint8_t pin = getOptionsInt(cribPinS);
  boolean state = digitalRead(pin);  // get the current state of GPIO1 pin
  ts.add(10, freq, [&](void*) {
    digitalWrite(pin, !state);     // set pin to the opposite state

  }, nullptr, true);
}

#endif
