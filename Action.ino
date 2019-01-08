#ifdef pinOutM
// ------------------- Инициализация Реле
void initRelay() {
  uint8_t pin = readArgsInt(); // первый аргумент pin
  pin =  pinTest(pin);
  String num = readArgsString(); // второй аргумент прификс реле 0 1 2
  boolean state = readArgsInt(); // третий  аргумент состояние на старте
  boolean inv = readArgsInt(); // четвертый аргумент инверсия выхода
  String title = readArgsString(); // Пятый аргумент подпись
  initPin(pin, num, state, inv, relayS, title);
  sCmd.addCommand(relayS.c_str(), relay); //
  commandsReg(relayS);
  actionsReg(relayS + num);
  modulesReg(relayS + num);
}

// ------------------- Инициализация PinOut
void initPinOut() {
  uint8_t pin = readArgsInt(); // первый аргумент pin
  pin =  pinTest(pin);
  String num = readArgsString(); // второй аргумент прификс pina 0 1 2
  boolean state = readArgsInt(); // третий  аргумент состояние на старте
  boolean inv = readArgsInt(); // четвертый аргумент инверсия выхода
  String title = readArgsString(); // Пятый аргумент подпись
  //String name = pinOutS;
  //initPin(pin, num, state, inv, name, title);
  initPin(pin, num, state, inv, pinOutS, title);
  sCmd.addCommand(pinOutS.c_str(), pinOut); //
  commandsReg(pinOutS);
  actionsReg(pinOutS + num);
  modulesReg(pinOutS + num);
}

void initPin(uint8_t pin, String num, boolean state, boolean inv, String name, String title) {
  String nameR = name + num;
  if (title == "") title = nameR;
  sendStatus(nameR, state);
  sCmd.readStr("wReg toggle " + nameR + " " + title);
  sendOptions(name + PinS + num, pin);
  sendOptions(name + NotS + num, inv);
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
}

// http://192.168.0.91/cmd?command=relay off 1
void relay() {
  String com = readArgsString(); // действие
  String num = readArgsString(); // номер реле
  pinSet(num, com, relayS);
}

// http://192.168.0.91/cmd?command=relay off 1
void pinOut() {
  String com = readArgsString(); // действие
  String num = readArgsString(); // номер реле
  pinSet(num, com, pinOutS);
}

void pinSet(String num, String com, String name) {
  String kayPin = name + PinS + num; // Получим имя ячейки пин по номеру
  String kay = name + num; // Имя реле
  uint8_t pin = getOptionsInt(kayPin); // Получим пин по Имени реле
  uint8_t inv = getOptionsInt(name + NotS + num); // Получим признак инверсии по Имени реле
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
#endif
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
  sCmd.addCommand(rfsendS.c_str(), handleRfTransmit);
  commandsReg(rfsendS);
  actionsReg(rfsendS);
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
  sCmd.addCommand(lvsendS.c_str(), handleRfLivolo);
  commandsReg(lvsendS);
  modulesReg("rfLivolo");
}

void handleRfLivolo() {
  int cod = readArgsInt();
  int len = readArgsInt();
  gLivolo->sendButton(cod, len);
}

#ifdef DimmerM
// ------------------- Инициализация Dimmer
void initDimmer() {
  uint8_t outPin = readArgsInt(); // первый аргумент pin
  outPin =  pinTest(outPin);
  sendOptions(dimmerS, outPin);
  uint8_t zcPin = readArgsInt(); // второй аргумент pin zerro
  zcPin =  pinTest(zcPin);
  pinMode(zcPin, INPUT_PULLUP);
  pinMode(outPin, OUTPUT);
  digitalWrite(outPin, 0);
  sendStatus(dimmerS, 0);
  hw_timer_init(FRC1_SOURCE, 0);
  hw_timer_set_func(dimTimerISR);
  attachInterrupt(zcPin, zcDetectISR, RISING);
  String title = readArgsString(); // Третий аргумент подпись
  sCmd.addCommand(dimmerS.c_str(), dimmerOn); //
  commandsReg(dimmerS);
  actionsReg(dimmerS);
  modulesReg(dimmerS);
}

void zcDetectISR() {

}
void dimTimerISR() {

}
void dimmerOn() {
  String com = readArgsString(); //состояние
  String brightness = readArgsString(); //яркость
  uint8_t state = getStatusInt(dimmerS);
  Serial.println(com);
  if (com == "on" || com == "1") {
    fade == 1;
    flag = sendStatus(dimmerS, 1);
  }
  if (com == "off" || com == "0") {
    fade == 0;
    flag = sendStatus(dimmerS, 0);
  }
  if (com == "not") {
    flag = sendStatus(dimmerS, !state);
  }
  if (com == "set") {
    tarBrightness = brightness.toInt();
  }
}

#endif
