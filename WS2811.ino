/*
   Модуль использует библиотеки
   #include <Adafruit_NeoPixel.h>       //https://github.com/adafruit/Adafruit_NeoPixel
   #include <WS2812FX.h>                //https://github.com/kitesurfer1404/WS2812FX
   Глобальные переменные
  int ledCount = 15;              // Количество лед огней
   Переменная статуса stateRGB
   Объект должен быть определен в начале скетча
   WS2812FX ws2812fx = WS2812FX(ledCount, RGB_PIN, NEO_GRB + NEO_KHZ800);
     В Loop обработчик
     ws2812fx.service();
*/

void initRGB() {
  configJson = jsonWrite(configJson, "colorRGB", "ff6600");
  configJson = jsonWrite(configJson, "speedRGB", 100);
  configJson = jsonWrite(configJson, "BrightnessRGB", 255);
  configJson = jsonWrite(configJson, "ModeRGB", 0);
  configJson = jsonWrite(configJson, "timeRGB", 0);
  configJson = jsonWrite(configJson, "timeBUZ", 0);
  //Serial.end();
  // Настраивается по запросу /set?
  HTTP.on("/set", handle_RGB);
  HTTP.on("/rgb", LedRGB);
  // Реагирует на комманду rgbnot
  sCmd.addCommand("rgbnot",    rgbNot);
  sCmd.addCommand("rgbon",    rgbOn);
  sCmd.addCommand("rgboff",    rgbOff);

  ws2812fx.setPin(readArgsInt());
  ws2812fx.updateLength(readArgsInt());
  ws2812fx.init();
  ws2812fx.setMode(jsonReadtoInt(configJson, "ModeRGB")); // Режим
  // Начальный цвет
  uint32_t  tmp = strtol(("0x" + jsonRead(configJson, "colorRGB")).c_str(), NULL, 0);
  if (tmp >= 0x000000 && tmp <= 0xFFFFFF) {
    ws2812fx.setColor(tmp);
  }
  ws2812fx.setSpeed(jsonReadtoInt(configJson, "speedRGB")); // Скорость
  ws2812fx.setBrightness(jsonReadtoInt(configJson, "BrightnessRGB")); //Яркость
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
  String BrightnessRGB = HTTP.arg("b");
  if (BrightnessRGB != "") {
    configJson = jsonWrite(configJson, "BrightnessRGB", BrightnessRGB);
    ws2812fx.setBrightness(BrightnessRGB.toInt());

  }
  //Получаем режим
  String ModeRGB = HTTP.arg("m");
  if (ModeRGB != "") {
    configJson = jsonWrite(configJson, "ModeRGB", ModeRGB);
    ws2812fx.setMode(ModeRGB.toInt() % ws2812fx.getModeCount());
  }
  //Получаем время таймера
  String  timeRGB = HTTP.arg("time");
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
