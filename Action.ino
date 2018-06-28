// ------------------- Инициализация Реле
void initRelay() {
  uint8_t pin = readArgsInt(); // первый аргумент pin
  pin =  pinTest(pin);
  boolean state = readArgsInt(); // второй аргумент состояние на старте
  String num = readArgsString(); // третьий аргумент прификс реле 0 1 2
  boolean inv = readArgsInt(); // четвертый аргумент инверсия выхода
  sendStatus(stateRelayS + num, state);
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
  sCmd.addCommand("relay",     relay); //
  commandsReg(relayS);
  modulesReg(relayS + num);
}

// http://192.168.0.91/cmd?command=relay off 1
void relay() {
  String com = readArgsString(); // действие
  String num = readArgsString(); // номер реле
  uint32_t times = readArgsInt(); // время
  String del = readArgsString(); // это delay
  String kayPin = relayPinS + num;
  String kay = stateRelayS + num;
  uint8_t state = getStatusInt(kay);
  uint8_t pin = getOptionsInt(kayPin);
  if (com == "on") {
    if (pin  > 19) {
      relayWrite(pin, 1 ^ getOptionsInt(relayNotS + num));
    } else
      digitalWrite(pin, 1 ^ getOptionsInt(relayNotS + num));
    flag = sendStatus(kay, 1);
  }
  if (com == "off") {
    if (pin  > 19) {
      relayWrite(pin, 0 ^ getOptionsInt(relayNotS + num));
    } else
      digitalWrite(pin, 0 ^ getOptionsInt(relayNotS + num));
    flag = sendStatus(kay, 0);
  }
  if (com == "not") {
    if (pin > 19) {
      relayWrite(pin, !state ^ getOptionsInt(relayNotS + num));
    } else
      digitalWrite(pin, !state ^ getOptionsInt(relayNotS + num));
    flag = sendStatus(kay, !state);
  }
  if (times != 0) {
    if (del == "t") {
      impulsTime(times - 1, "relay not " + num);
    } else {
      comTimeP = "relay not " + num;
      String t = GetTime();
      pTime = timeToString(timeToLong(t) + (times-1));
    }
    statusS = relayStatus(configJson, kay);
  }
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


// читает данные из раздела state строки json и возвращает строку для смены класса кнопки
String relayStatus(String json, String state) {
  String out = "{}";
  if (jsonReadToInt(json, state)) {
    jsonWrite(out, titleS, "{{LangOff}}");
    jsonWrite(out, classS, btnS + infoS);
  }
  else {
    jsonWrite(out, titleS, "{{LangOn}}");
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


// ----------------------Передатчик ИK
void irTransmitter() {
  String moduleName = "irTransmitter";
  byte pin = readArgsInt();
  pin =  pinTest(pin);
  irSender = new IRsend(pin);  // Create a new IRrecv object. Change to what ever pin you need etc.
  irSender->begin();
  sCmd.addCommand("irsend", handleIrTransmit);
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
  pin =  pinTest(pin);
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
  pin =  pinTest(pin);
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

// ------------------- Инициализация Buzera
void initBuzzer() {
  uint8_t pin = readArgsInt(); // первый аргумент pin
  pin =  pinTest(pin);
  sendOptions(buzzerPinS, pin);
  sCmd.addCommand("tone", buzzerTone);
  commandsReg("tone");
  modulesReg("tone");
}

void buzzerTone() {
  int freq = readArgsInt();
  int duration = readArgsInt();
  uint8_t pin = getOptionsInt(buzzerPinS);
  tone(pin, freq, duration);
}


#ifdef CRIB
// ------------------- Инициализация кроватка
void initCrib() {
  uint8_t pin = readArgsInt(); // первый аргумент pin
  pin =  pinTest(pin);
  uint8_t pin1 = readArgsInt(); // первый аргумент pin
  pin1 =  pinTest(pin1);
  pinMode(pin, OUTPUT);
  pinMode(pin1, OUTPUT);
  sendOptions("cribPin", pin);
  sendOptions("cribPin1", pin1);
  sCmd.addCommand("crib", startCrib);
  commandsReg("crib");
  modulesReg("crib");
}

void startCrib() {
  int freq = readArgsInt();
  int duration = readArgsInt();
  flipper.attach_ms(freq, flip, duration);
}


void flip(int duration)
{
  static int count = 0;
  uint8_t pin = getOptionsInt("cribPin");
  uint8_t pin1 = getOptionsInt("cribPin1");
  boolean state = digitalRead(pin);  // get the current state of GPIO1 pin

  digitalWrite(pin, !state);     // set pin to the opposite state
  digitalWrite(pin1, state);     // set pin to the opposite state

  ++count;
  // when the counter reaches a certain value, start blinking like crazy
  if (count == duration)
  {
    flipper.detach();
    count = 0;
  }

}
#endif
