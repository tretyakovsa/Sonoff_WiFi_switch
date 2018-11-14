// ------------------- Инициализация Реле
void initRelay() {
  uint8_t pin = readArgsInt(); // первый аргумент pin
  pin =  pinTest(pin);
  String num = readArgsString(); // второй аргумент прификс реле 0 1 2
  boolean state = readArgsInt(); // третий  аргумент состояние на старте
  boolean inv = readArgsInt(); // четвертый аргумент инверсия выхода
  String title = readArgsString(); // Пятый аргумент подпись
  String nameR = relayS + num;
  if (title == "") title = nameR;
  //sendStatus(nameR, state);
  sCmd.readStr("wReg toggle " + nameR + " " + title);
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
  sCmd.addCommand(relayS.c_str(), relay); //
  commandsReg(relayS);
  actionsReg(relayS + num);
  modulesReg(relayS + num);
}

// http://192.168.0.91/cmd?command=relay off 1
void relay() {
  String com = readArgsString(); // действие
  String num = readArgsString(); // номер реле
  uint32_t times = readArgsInt(); // время
  relaySet(num, com);
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

#ifdef CRIB
// ------------------- Инициализация кроватка
void initPuls() {
  sCmd.addCommand(cribS.c_str(), startPuls);
  commandsReg(cribS);
  modulesReg(cribS);
}

void startPuls() {
  String com = readArgsString(); // on off
  if (com != "") { // если комманда есть
    String pulseCom = readArgsString(); // Команда relay3 или rgb
    String tacks = jsonRead(pulsList, pulseCom);  //Получим нимер задачи для устройства
    sendOptions(pulseS+"State"+tacks, true);
    pulseCom = topicToCom(pulseCom);   // Пробел между командой и номером
    pulseCom.replace(" ", " not ");    // Модефицируем командув not
    sendOptions(pulseComS+tacks, pulseCom); // Сохраним команду
    String pulseComT = pulseCom;            // Здесь будет комманда off
    pulseComT.replace(notS, offS);          // Модефицируем командув off
    if (com == onS || com == "1") {         // Если комманда есть
      int freq = readArgsInt(); // Как долго включен
      sendOptions(pulseS+tacks+"0", freq);
      if (freq != 0) {
        String temp = readArgsString(); // Как долго выключен
        int freq1 = temp.toInt();
        if (temp == "" || temp == "-")freq1 = freq;
        if (temp == 0)freq1 = 0;
        sendOptions(pulseS+tacks+"1", freq1);
        int period = freq + freq1;
        String pulseTime = readArgsString(); // Время работы
        if (pulseTime.lastIndexOf("s") > -1) {
          pulseTime.replace("s", "000");
        }
        if (pulseTime.lastIndexOf("i") > -1) {
          int temp1 = pulseTime.toInt();
          pulseTime.replace("i", "");
          pulseTime = "";
          pulseTime += (period) * temp1;
        }
        int pulseTimeInt = pulseTime.toInt();
        int remainder = pulseTimeInt % (period);
        if (remainder > period / 2) {
          pulseTimeInt += period-remainder;
        } else  pulseTimeInt -= remainder;
        sCmd.readStr(pulseComT);
        sendOptions(pulseTimeS+tacks, pulseTimeInt);
        imPuls(tacks.toInt());
      }
    }
    if (com == "off" || com == "0") {
      pulseCom.replace(notS, offS);
      sCmd.readStr(pulseCom);
      flipper[tacks.toInt()].detach();
    }
  }
}

void imPuls(int tacks) {
  String pulseStateN= "pulseState"+(String)tacks;
  boolean stopF = true;
  String pulseCom = getOptions(pulseComS+tacks);             // Получить каким устройством управляем
  String pulseTime = getOptions(pulseTimeS+tacks);           // Получим текстовое значние времени работы
  int pulseTimeInt = pulseTime.toInt();                 // Получим int значние времени работы
  uint8_t low = getOptionsInt(pulseStateN);
  int timeOn = getOptionsInt(pulseS+tacks+low);     // Время включено
  int timeOff = getOptionsInt(pulseS+tacks+!low);   // Время выключено
  if (timeOn > 0) {                                     // Если время включено >0 сразу закончить
    sCmd.readStr(pulseCom);                             // Выполнить команду
    if (pulseTime != "null" && pulseTimeInt != 0 ) {
      sendOptions(pulseTimeS+tacks, (String)(pulseTimeInt - timeOn));
      if (getOptionsInt(pulseTimeS+tacks) <= 0) {
        flipper[tacks].detach();
        stopF = false;
      }
    }
    low = !low;
    sendOptions(pulseStateN, low);
    if (stopF) {
      flipper[tacks].attach_ms(timeOn, imPuls, tacks);               // Задать время через которое процедура будет вывана повторно
    }
  } else {
    sCmd.readStr(pulseCom);                            // Выключить
    flipper[tacks].detach();                               // Остановим таймер
    //low = false;                                      // Сбросить флаг ???
    sendOptions(pulseStateN, false);
  }
}

#endif
