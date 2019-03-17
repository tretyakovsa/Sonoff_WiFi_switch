/*
   Модуль использует библиотеки
   #include <Adafruit_NeoPixel.h>       //https://github.com/adafruit/Adafruit_NeoPixel
   #include <WS2812FX.h>                //https://github.com/MTJoker/WS2812FX
   Объект должен быть определен в начале скетча
   WS2812FX ws2812fx = WS2812FX();
     В Loop обработчик
     ws2812fx.service();
*/
#ifdef rgbM
void initRGB() {
  // Реагирует на комманду rgbnot
  sCmd.addCommand(rgbS.c_str(),    rgb);
  actionsReg(rgbS);
  commandsReg(rgbS);

  byte pin = readArgsInt();
  pin =  pinTest(pin);
  ws2812fx.setPin(pin);
  ws2812fx.updateLength(readArgsInt());
  int state = readArgsInt(); //состояние
  sendStatus(rgbS, state);
  sendStatus(colorRGBS,  readArgsString()); //цвет
  sendStatus(speedRGBS, readArgsInt()); //скорость
  sendStatus(brightnessRGBS, readArgsInt()); //яркость
  sendStatus(modeRGBS, readArgsInt()); //режим
  String title = readArgsString(); // Пятый аргумент подпись
  if (title == "" ) title = rgbS;
  sendStatus(stateRGBS, state);
  sCmd.readStr("wReg toggle " + rgbS + " " + title);
  ws2812fx.init();
  ws2812fx.setMode(getStatusInt(modeRGBS)); // Режим
  setColorString(colorRGBS);   // Начальный цвет
  int temp = getStatusInt(speedRGBS);
  temp = map(temp, 0, 255, 65535, 10);
  ws2812fx.setSpeed(temp); // Скорость
  ws2812fx.setBrightness(getStatusInt(brightnessRGBS)); //Яркость
  //регистрируем модуль

  if (state) {
    ws2812fx.start();
  }

  modulesReg(rgbS);
}

void setColorString(String color) {
  uint32_t   tmp = strtol(("0x" + getStatus(color)).c_str(), NULL, 0);
  if (tmp >= 0x000000 && tmp <= 0xFFFFFF) {
    ws2812fx.setColor(tmp);
  }
}

void rgb() {
  // PIN LED ленты, вторая - количество лед, терья - состояние LED при вкл
  String com = readArgsString(); //состояние
  String color = readArgsString(); //цвет
  String speed = readArgsString(); //скорость
  String brightness = readArgsString(); //яркость
  String mode = readArgsString(); //режим
  uint8_t temp;
  //String kay = stateRGBS;
  uint8_t state = getStatusInt(stateRGBS);
  uint32_t times = color.toInt();

  if (com == "set") {
    if (color != emptyS) {
      if (color == "-") {
      } else {
        sendStatus(colorRGBS,  color);
        setColorString(colorRGBS);
      }

    }
    if (speed != emptyS) {
      if (speed == "-") {}
      else {
        temp = speed.toInt();
        temp = map(temp, 0, 255, 65535, 10);
        sendStatus(speedRGBS,  temp);
        ws2812fx.setSpeed(temp);
      }
    }
    if (brightness != emptyS) {
      if (brightness == "-") {}
      else {
        temp = brightness.toInt();
        sendStatus(brightnessRGBS,  temp);
        ws2812fx.setBrightness(temp);
      }
    }
    if (mode != emptyS) {
      if (mode == "-") {}
      else {
        temp = mode.toInt();
        sendStatus(modeRGBS,  temp);
        ws2812fx.setMode(temp);
      }
    }
    ws2812fx.start();
    flag = sendStatus(stateRGBS, 1);
  } else {
    if (com == "on" || com == "1") {
      ws2812fx.start();
      flag = sendStatus(stateRGBS, 1);
    }
    if (com == "off" || com == "0") {
      ws2812fx.stop();
      flag = sendStatus(stateRGBS, 0);
    }
    if (com == "not") {
      flag = sendStatus(stateRGBS, !state);
      if (state) {
        ws2812fx.stop();
      }
      else {
        ws2812fx.stop();
        ws2812fx.start();
      }
    }
  }

  //statusS = relayStatus(configJson, stateRGBS);
  statusS = htmlStatus(configJson, stateRGBS, langOnS, langOffS);
}
#endif
// Шим RGB лента
#ifdef rgbShimM
void initRGBSHIM() {
  // Реагирует на комманду rgbnot
  sCmd.addCommand(rgbSS.c_str(),    rgbShim);
  actionsReg(rgbSS);
  commandsReg(rgbSS);
  for (uint8_t i = 0; i <= 2; i++) {
    byte pin = readArgsInt();
    pin =  pinTest(pin);
    sendOptions("rgbSPin" + String(i), pin);
    pinMode(pin, OUTPUT);
  }
  int state = readArgsInt(); //состояние
  sendStatus(rgbSS, state);
  String color = readArgsString();
  sendStatus(colorSRGBS,  color); //цвет
  sendStatus(speedSRGBS, readArgsInt()); //скорость
  sendStatus(brightnessSRGBS, readArgsInt()); //яркость
  //sendStatus(modeSRGBS, readArgsInt()); //режим
  String title = readArgsString(); // Пятый аргумент подпись
  if (title == "" ) title = rgbSS;
  sCmd.readStr("wReg toggle " + rgbSS + " " + title);

  //регистрируем модуль

  if (state) {
    setColorSString(color);
  }
  modulesReg(rgbSS);
}
void setColorSString(String color) {
  if (color.length() == 6) {
    analogWrite(getOptionsInt("rgbSPin0"), map(hexToUint32(color.substring(0, 2)), 0, 255, 0, 1023));
    analogWrite(getOptionsInt("rgbSPin1"), map(hexToUint32(color.substring(2, 4)), 0, 255, 0, 1023));
    analogWrite(getOptionsInt("rgbSPin2"), map(hexToUint32(color.substring(4, 6)), 0, 255, 0, 1023));
  }
}

uint32_t hexToUint32(String str) {
  return strtol(("0x" + str).c_str(), NULL, 0);
}

void rgbShim() {
  // PIN LED ленты, вторая - количество лед, терья - состояние LED при вкл
  String com = readArgsString(); //состояние
  String color = readArgsString(); //цвет
  String speed = readArgsString(); //скорость
  String brightness = readArgsString(); //яркость
  String mode = readArgsString(); //режим
  uint8_t temp;
  uint32_t times = color.toInt();
  uint8_t state = getStatusInt(rgbSS);
  if (com == "on" || com == "1") {

    setColorSString(getStatus(colorSRGBS));
    flag = sendStatus(rgbSS, 1);
  }
  if (com == "off" || com == "0") {

    setColorSString("000000");
    flag = sendStatus(rgbSS, 0);
  }
  if (com == "not") {

    flag = sendStatus(rgbSS, !state);
    if (state) {
      setColorSString("000000");
    }
    else {
      setColorSString(getStatus(colorSRGBS));
    }
  }

  if (com == "set") {
    if (color != emptyS) {
      sendStatus(colorSRGBS,  color);
      setColorSString(color);
    }
    if (speed != emptyS) {
      temp = speed.toInt();
      sendStatus(speedSRGBS,  temp);
      //
    }
    if (brightness != emptyS) {
      temp = brightness.toInt();
      sendStatus(brightnessSRGBS,  temp);
      //
    }
    if (mode != emptyS) {
      temp = mode.toInt();
      sendStatus(modeSRGBS,  temp);
      //
    }
    setColorSString(getStatus(colorSRGBS));
    flag = sendStatus(rgbSS, 1);
  }
  //statusS = relayStatus(configJson, rgbSS);
  statusS = htmlStatus(configJson, rgbSS, langOnS, langOffS);
}
#endif
#ifdef JalousieM
// ---------------- Управление двигателем жалюзи
void initJalousie() {
  // Сенсор будет работать по прерыванию
  int pinTurn = readArgsInt();
  pinTurn =  pinTest(pinTurn);
  attachInterrupt(pinTurn, turn_0, FALLING );
  sendOptions(pinTurnS, pinTurn);
  int pinMotor1 = readArgsInt();
  int pinMotor2 = readArgsInt();
  pinMotor1 =  pinTest(pinMotor1);
  pinMotor2 =  pinTest(pinMotor2);
  sendOptions(pinMotorS + "1", pinMotor1);
  sendOptions(pinMotorS + "2", pinMotor2);
  pinMode(pinMotor1, OUTPUT);
  pinMode(pinMotor2, OUTPUT);
  setMotor(HIGH, HIGH);
  sendStatus(stateJalousieS, 1);
  sendStatus(turnSensorS, 0);
  setupToOptions(turnS);
  //sendOptions(turnS, jsonReadToInt(configSetup, turnS));
  // open close not stop turn
  sCmd.addCommand(stateJalousieS.c_str(),     jalousie);
  commandsReg(stateJalousieS);
  actionsReg(stateJalousieS);
  modulesReg(stateJalousieS);
}

// Выполняется при вращение сенсора
void turn_0() {
  static unsigned long millis_prev;
  // Устроняем дребезг контакта
  if (millis() - 500 > millis_prev) {
    //Текущее состояние оборотов
    int turnSensor = jsonReadToInt(configJson, turnSensorS);
    turnSensor++; // счетчик количества оборотов
    flag = sendStatus(turnSensorS, turnSensor);
    int turn = jsonReadToInt(configSetup, turnS);
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
    sendOptions(turnS, jsonReadToInt(configSetup, turnS));
    saveConfigSetup ();
  }
  //statusS = jalousieStatus(configJson, stateJalousieS);
  statusS = htmlStatus(configJson, stateJalousieS, LangOpenS, LangCloseS);
}

void setMotor(boolean a, boolean b) {
  digitalWrite(getOptionsInt(pinMotorS + "1"), a);
  digitalWrite(getOptionsInt(pinMotorS + "2"),  b);
}
#endif
