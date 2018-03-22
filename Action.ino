// ------------------- Инициализация Реле
void initRelay() {
  uint8_t pin = readArgsInt(); // первый аргумент pin
  boolean state = readArgsInt(); // второй аргумент состояние на старте
  String num = readArgsString(); // третьий аргумент прификс реле 0 1 2
  boolean inv = readArgsInt(); // четвертый аргумент инверсия выхода
  sendStatus(stateRelayS + num, state);
  sendOptions(relayPinS + num, pin);
  sendOptions(relayNotS + num, inv);
  pinMode(pin, OUTPUT);
  digitalWrite(pin, state^inv);
  sCmd.addCommand("relay",     relay); //
  commandsReg(relayS);
  modulesReg(relayS+num);
  //modulesReg(relayS); // для совместимости со старым кодом
}

// http://192.168.0.91/cmd?command=relay%20off%201
void relay() {
  String com = readArgsString();
  String num = readArgsString();
  String kayPin = relayPinS + num;
  String kay = stateRelayS + num;
  uint8_t state = getStatusInt(kay);
  if (com == "on") {
    digitalWrite(getOptionsInt(kayPin), 1^getOptionsInt(relayNotS + num));
    flag = sendStatus(kay, 1);
      }
  if (com == "off") {
    digitalWrite(getOptionsInt(kayPin), 0^getOptionsInt(relayNotS + num));
    flag = sendStatus(kay, 0);
  }
  if (com == "not") {
    digitalWrite(getOptionsInt(kayPin), !state^getOptionsInt(relayNotS + num));
    flag = sendStatus(kay, !state);
  }
  statusS = relayStatus(configJson, kay);
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

// читает данные из раздела state строки json и возвращает строку для смены класса кнопки
String relayStatus(String json, String state) {
  String out = "{}";
  if (jsonReadtoInt(json, state)) {
    out = jsonWrite(out, titleS, "{{LangOff}}");
    out = jsonWrite(out, classS, btnS+infoS);
  }
  else {
    out = jsonWrite(out, titleS, "{{LangOn}}");
    out = jsonWrite(out, classS, btnS+primaryS);

  }
  return out;
}

/*
 Serial.write(0xA0);
  Serial.write(0x04);
  Serial.write(0xxx);
  Serial.write(0xA1);
  Serial.flush();
   где вместо "0xxx" - 0 (выключить оба реле), 1 (включить одно реле), 2 (включить второе реле), 3 (включить оба реле), то можно таким образом управлять релюшками.
  */


// ----------------------Передатчик ИK
void irTransmitter() {
  String moduleName = "irTransmitter";
  byte pin = readArgsInt();
  if (pin == 1 || pin == 3)  Serial.end();
  irSender = new IRsend(pin);  // Create a new IRrecv object. Change to what ever pin you need etc.
  irSender->begin();
  sCmd.addCommand("irsend",handleIrTransmit);
  commandsReg(irsendS);
  modulesReg(moduleName);

}

void handleIrTransmit() {
  uint32_t  tmp = strtol(("0x" + readArgsString()).c_str(), NULL, 0);
  irSender->sendNEC(tmp, 32);
}


// ----------------------Передатчик на 433мГ
void rfTransmitter() {
  byte pin = readArgsInt();
  if (pin == 1 || pin == 3)  Serial.end();
  mySwitch.enableTransmit(pin);
  Serial.println(pin);
  sCmd.addCommand("rfsend",handleRfTransmit);
  commandsReg("rfsend");
  modulesReg("rfTransmitter");
}

void handleRfTransmit() {
  Serial.println("rf");
    int cod = readArgsInt();
    int len = readArgsInt();
    if (len==0) len = 24;
    mySwitch.send(cod, len);
}

// ----------------------Передатчик Livolvo на 433мГ
void rfLivolo() {
  byte pin = readArgsInt();
  if (pin == 1 || pin == 3)  Serial.end();
  gLivolo = new LivoloTx(pin);
  sCmd.addCommand("lvsend",handleRfLivolo);
  commandsReg("lvsend");
  modulesReg("rfLivolo");
}


void handleRfLivolo() {
    int cod = readArgsInt();
    int len = readArgsInt();
    gLivolo->sendButton(cod, len);
}

// ------------------- Инициализация Buzera
void initBuzzer() {
  uint8_t pin = readArgsInt(); // первый аргумент pin
  sendOptions(buzzerPinS, pin);
  sCmd.addCommand("tone", buzzerTone);
  commandsReg("tone");
  modulesReg("tone");
}

void buzzerTone(){
 int freq = readArgsInt();
 int duration = readArgsInt();
 uint8_t pin = getOptionsInt(buzzerPinS);
 tone(pin, freq, duration);
  }

