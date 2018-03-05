/*
   Модуль использует библиотеки
   #include <Adafruit_NeoPixel.h>       //https://github.com/adafruit/Adafruit_NeoPixel
   #include <WS2812FX.h>                //https://github.com/MTJoker/WS2812FX
   Объект должен быть определен в начале скетча
   WS2812FX ws2812fx = WS2812FX();
     В Loop обработчик
     ws2812fx.service();
*/

void initRGB() {
  // Реагирует на комманду rgbnot
  sCmd.addCommand("rgb",    rgb);

  commandsReg(rgbS);

  byte pin = readArgsInt();
  if (pin == 1 || pin == 3)  Serial.end();
  ws2812fx.setPin(pin);
  ws2812fx.updateLength(readArgsInt());
  int state = readArgsInt(); //состояние
  sendStatus(stateRGBS, state);
  sendStatus(colorRGBS,  readArgsString()); //цвет
  sendStatus(speedRGBS, readArgsInt()); //скорость
  sendStatus(brightnessRGBS, readArgsInt()); //яркость
  sendStatus(modeRGBS, readArgsInt()); //режим
  sendStatus("timeRGB", 0);
  sendStatus("timeBUZ", 0);
  ws2812fx.init();
  ws2812fx.setMode(getStatusInt(modeRGBS)); // Режим
  setColorString(colorRGBS);   // Начальный цвет
  ws2812fx.setSpeed(getStatusInt(speedRGBS)); // Скорость
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
  if (color != "") {
    sendStatus(colorRGBS,  color);
    setColorString(colorRGBS);
  }
  if (speed != "") {
    temp = speed.toInt();
    sendStatus(speedRGBS,  temp);
    ws2812fx.setSpeed(temp);
  }
  if (brightness != "") {
    temp = brightness.toInt();
    sendStatus(brightnessRGBS,  temp);
    ws2812fx.setBrightness(temp);
  }
  if (mode != "") {
    temp = mode.toInt();
    sendStatus(modeRGBS,  temp);
    ws2812fx.setMode(temp);
  }
  //String kay = stateRGBS;
  uint8_t state = getStatusInt(stateRGBS);
  if (com == "on") {
    ws2812fx.start();
    flag = sendStatus(stateRGBS, 1);
  }
  if (com == "off") {
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
  statusS = relayStatus(configJson, stateRGBS);
}

// Шим RGB лента
void initRGBSHIM() {
  // Реагирует на комманду rgbnot
  sCmd.addCommand("rgbs",    rgbShim);
  commandsReg(rgbSS);
  for (uint8_t i = 0; i <= 2; i++) {
    byte pin = readArgsInt();
    if (pin == 1 || pin == 3)  Serial.end();
    sendOptions("rgbSPin" + String(i), pin);
    pinMode(pin, OUTPUT);
  }
  int state = readArgsInt(); //состояние
  sendStatus(stateSRGBS, state);
  String color = readArgsString();
  sendStatus(colorSRGBS,  color); //цвет
  sendStatus(speedSRGBS, readArgsInt()); //скорость
  sendStatus(brightnessSRGBS, readArgsInt()); //яркость
  sendStatus(modeSRGBS, readArgsInt()); //режим


  //регистрируем модуль

  if (state) {
    setColorSString(color);
  }
  modulesReg(rgbSS);
}
void setColorSString(String color) {
  if (color.length() == 6) {
    analogWrite(getOptionsInt("rgbSPin0"), hexToUint32(color.substring(0, 2)));
    analogWrite(getOptionsInt("rgbSPin1"), hexToUint32(color.substring(2, 4)));
    analogWrite(getOptionsInt("rgbSPin2"), hexToUint32(color.substring(4, 6)));
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
  if (color != "") {
    sendStatus(colorSRGBS,  color);
    setColorSString(color);
  }
  if (speed != "") {
    temp = speed.toInt();
    sendStatus(speedSRGBS,  temp);
    //
  }
  if (brightness != "") {
    temp = brightness.toInt();
    sendStatus(brightnessSRGBS,  temp);
    //
  }
  if (mode != "") {
    temp = mode.toInt();
    sendStatus(modeSRGBS,  temp);
    //
  }
   uint8_t state = getStatusInt(stateSRGBS);
  if (com == "on") {
    setColorSString(getStatus(colorSRGBS));
    flag = sendStatus(stateSRGBS, 1);
  }
  if (com == "off") {
    setColorSString("000000");
    flag = sendStatus(stateSRGBS, 0);
  }
  if (com == "not") {
    flag = sendStatus(stateSRGBS, !state);
    if (state) {
      setColorSString("000000");
    }
    else {
     setColorSString(getStatus(colorSRGBS));
    }
  }
  statusS = relayStatus(configJson, stateSRGBS);
}




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
