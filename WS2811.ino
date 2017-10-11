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
  sendStatus("colorRGB",  "ff6600");
  sendStatus("speedRGB", 100);
  sendStatus("brightnessRGB", 255);
  sendStatus("modeRGB", 0);
  sendStatus("timeRGB", 0);
  sendStatus("timeBUZ", 0);
  // Настраивается по запросу /set?
  HTTP.on("/set", handle_RGB);
  HTTP.on("/rgb", LedRGB);
  HTTPWAN.on("/set", handle_RGB);
  HTTPWAN.on("/rgb", LedRGB);

  // Реагирует на комманду rgbnot
  sCmd.addCommand("rgbnot",    rgbNot);
  sCmd.addCommand("rgbon",    rgbOn);
  sCmd.addCommand("rgboff",    rgbOff);
  commandsReg("rgbon","rgb");
  commandsReg("rgboff","rgb");
  commandsReg("rgbnot","rgb");
  byte pin = readArgsInt();
  if (pin == 1 || pin == 3)  Serial.end();
  ws2812fx.setPin(pin);
  ws2812fx.updateLength(readArgsInt());
  ws2812fx.init();
  ws2812fx.setMode(getStatusInt("modeRGB")); // Режим
  // Начальный цвет
  uint32_t  tmp = strtol(("0x" + jsonRead(configJson, "colorRGB")).c_str(), NULL, 0);
  if (tmp >= 0x000000 && tmp <= 0xFFFFFF) {
    ws2812fx.setColor(tmp);
  }
  ws2812fx.setSpeed(getStatusInt("speedRGB")); // Скорость
  ws2812fx.setBrightness(getStatusInt("brightnessRGB")); //Яркость
  //регистрируем модуль
  int state = readArgsInt();
  sendStatus("stateRGB", state);
  if (state) {command = "rgbon";}
  modulesReg("rgb");
}

//void handle_RGB() {}

// ----------------Задать режимы ленты
void handle_RGB() {
  //Получаем цвет
  String colorRGB = HTTP.arg("c");
  if (colorRGB != "") {
    flag = sendStatus("colorRGB", colorRGB);
    uint32_t  tmp = strtol(("0x" + colorRGB).c_str(), NULL, 0);
    if (tmp >= 0x000000 && tmp <= 0xFFFFFF) {
      ws2812fx.setColor(tmp);
    }
  }
  //Получаем скорость
  String speedRGB = HTTP.arg("s");
  if (speedRGB != "") {
    flag = sendStatus("speedRGB", speedRGB.toInt());
    ws2812fx.setSpeed(speedRGB.toInt());
  }
  //Получаем яркость
  String brightnessRGB = HTTP.arg("b");
  if (brightnessRGB != "") {
    flag = sendStatus("brightnessRGB", brightnessRGB.toInt());
    ws2812fx.setBrightness(brightnessRGB.toInt());

  }
  //Получаем режим
  String modeRGB = HTTP.arg("m");
  if (modeRGB != "") {
    flag = sendStatus("modeRGB", modeRGB.toInt());
    ws2812fx.setMode(modeRGB.toInt() % ws2812fx.getModeCount());
  }
  //Получаем время таймера
  String timeRGB = HTTP.arg("time");
  if (timeRGB != "") {
    flag = sendStatus("timeRGB", timeRGB.toInt());
  }
  //Получаем время сигнала
  String timeBUZ = HTTP.arg("s");
  if (timeBUZ != "") {
    flag = sendStatus("timeBUZ", timeBUZ.toInt());
  }
  command = "rgbon";
  String  state = "{}";
  state = jsonWrite(state, "title", "{{LangOff}}");
  state = jsonWrite(state, "class", "btn btn-block btn-lg btn-info");
  HTTP.send(200, "text/plain", state);
}

void rgbNot() {
  flag = sendStatus("stateRGB", !getStatusInt("stateRGB"));
  if (!getStatusInt("stateRGB")){
    ws2812fx.stop();
    }
    else{
    ws2812fx.stop();
    ws2812fx.start();
      }
  topicPub("/RGB_not/status", String(getStatusInt("stateRGB")), 1 );
}

void rgbOn() {

  if (!getStatusInt("stateRGB")) {
    configJson = jsonWrite(configJson, "stateRGB", 1);
    flag = sendStatus("stateRGB", 1);
    ws2812fx.start();
  }

}

void rgbOff() {

 if (getStatusInt("stateRGB")) {
    configJson = jsonWrite(configJson, "stateRGB", 1);
    flag = sendStatus("stateRGB", 0);
    ws2812fx.stop();
  }

}

void LedRGB() {
  command = "rgbnot";
  String state = "{}";
  if (getStatusInt("stateRGB")) {
    state = jsonWrite(state, "title", "{{LangOn}}");
    state = jsonWrite(state, "class", "btn btn-block btn-lg btn-primary");
  }
  else {
    state = jsonWrite(state, "title", "{{LangOff}}");
    state = jsonWrite(state, "class", "btn btn-block btn-lg btn-info");
  }
  HTTP.send(200, "text/plain", state);
}
