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
  configJson = jsonWrite(configJson, "colorRGB", "ff6600");
  configJson = jsonWrite(configJson, "speedRGB", 100);
  configJson = jsonWrite(configJson, "brightnessRGB", 255);
  configJson = jsonWrite(configJson, "modeRGB", 0);
  configJson = jsonWrite(configJson, "timeRGB", 0);
  configJson = jsonWrite(configJson, "timeBUZ", 0);
  //Serial.end();
  // Настраивается по запросу /set?
  HTTP.on("/set", handle_RGB);
  HTTP.on("/rgb", LedRGB);
  HTTPWAN.on("/set", handle_RGB);
  HTTPWAN.on("/rgb", LedRGB);

  // Реагирует на комманду rgbnot
  sCmd.addCommand("rgbnot",    rgbNot);
  sCmd.addCommand("rgbon",    rgbOn);
  sCmd.addCommand("rgboff",    rgbOff);

  ws2812fx.setPin(readArgsInt());
  ws2812fx.updateLength(readArgsInt());
  ws2812fx.init();
  ws2812fx.setMode(jsonReadtoInt(configJson, "modeRGB")); // Режим
  // Начальный цвет
  uint32_t  tmp = strtol(("0x" + jsonRead(configJson, "colorRGB")).c_str(), NULL, 0);
  if (tmp >= 0x000000 && tmp <= 0xFFFFFF) {
    ws2812fx.setColor(tmp);
  }
  ws2812fx.setSpeed(jsonReadtoInt(configJson, "speedRGB")); // Скорость
  ws2812fx.setBrightness(jsonReadtoInt(configJson, "brightnessRGB")); //Яркость
  //регистрируем модуль
  modulesReg("rgb");
}

//void handle_RGB() {}

// ----------------Задать режимы ленты
void handle_RGB() {
  //Получаем цвет
  String colorRGB = HTTP.arg("c");
  if (colorRGB != "") {
    configJson = jsonWrite(configJson, "colorRGB", colorRGB);
    uint32_t  tmp = strtol(("0x" + colorRGB).c_str(), NULL, 0);
    if (tmp >= 0x000000 && tmp <= 0xFFFFFF) {
      ws2812fx.setColor(tmp);
    }
  }
  //Получаем скорость
  String speedRGB = HTTP.arg("s");
  if (speedRGB != "") {
    configJson = jsonWrite(configJson, "speedRGB", speedRGB);
    ws2812fx.setSpeed(speedRGB.toInt());
  }
  //Получаем яркость
  String brightnessRGB = HTTP.arg("b");
  if (brightnessRGB != "") {
    configJson = jsonWrite(configJson, "brightnessRGB", brightnessRGB);
    ws2812fx.setBrightness(brightnessRGB.toInt());

  }
  //Получаем режим
  String modeRGB = HTTP.arg("m");
  if (modeRGB != "") {
    configJson = jsonWrite(configJson, "modeRGB", modeRGB);
    ws2812fx.setMode(modeRGB.toInt() % ws2812fx.getModeCount());
  }
  //Получаем время таймера
  String timeRGB = HTTP.arg("time");
  if (timeRGB != "") {
    configJson = jsonWrite(configJson, "timeRGB", timeRGB);
  }
  //Получаем время сигнала
  String timeBUZ = HTTP.arg("s");
  if (timeBUZ != "") {
    configJson = jsonWrite(configJson, "timeBUZ", timeBUZ);
  }
  command = "rgbon";
  String  state = "{}";
  state = jsonWrite(state, "title", "{{LangOff}}");
  state = jsonWrite(state, "class", "btn btn-block btn-lg btn-info");
  HTTP.send(200, "text/plain", state);
}


void rgbNot() {

  if (jsonReadtoInt(configJson, "stateRGB")) {
    ws2812fx.stop();
  }
  else {
    ws2812fx.stop();
    ws2812fx.start();
  }
  configJson = jsonWrite(configJson, "stateRGB", !jsonReadtoInt(configJson, "stateRGB"));

}

void rgbOn() {

  if (!jsonReadtoInt(configJson, "stateRGB")) {
    ws2812fx.start();
    configJson = jsonWrite(configJson, "stateRGB", 1);

  }

}

void rgbOff() {

  if (jsonReadtoInt(configJson, "stateRGB")) {
    ws2812fx.stop();
    configJson = jsonWrite(configJson, "stateRGB", 0);

  }

}

void LedRGB() {
  command = "rgbnot";
  String state = "{}";
  if (jsonReadtoInt(configJson, "stateRGB")) {
    state = jsonWrite(state, "title", "{{LangOn}}");
    state = jsonWrite(state, "class", "btn btn-block btn-lg btn-primary");
  }
  else {
    state = jsonWrite(state, "title", "{{LangOff}}");
    state = jsonWrite(state, "class", "btn btn-block btn-lg btn-info");
  }
  HTTP.send(200, "text/plain", state);
}
