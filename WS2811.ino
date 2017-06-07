/*
   Модуль использует библиотеки
   #include <Adafruit_NeoPixel.h>       //https://github.com/adafruit/Adafruit_NeoPixel
   #include <WS2812FX.h>                //https://github.com/kitesurfer1404/WS2812FX
   Глобальные переменные
  String colorRGB = "ff6600";
  String speedRGB = "100";
  String BrightnessRGB = "255";
  String ModeRGB = "0";
  String timeRGB = "";
  String timeBUZ = "";
  int stateRGB = 0;
  int ledCount = 15;              // Количество лед огней
   Переменная статуса stateRGB
   Объект должен быть определен в начали скетча
   WS2812FX ws2812fx = WS2812FX(ledCount, RGB_PIN, NEO_GRB + NEO_KHZ800);
     В Loop обработчик
     ws2812fx.service();
*/

void initRGB() {
  //Serial.end();
  // Настраивается по запросу /set?
  HTTP.on("/set", handle_RGB);
  // Реагирует на комманду rgbnot
  sCmd.addCommand("rgbnot",    rgbNot);
  sCmd.addCommand("rgbon",    rgbOn);
  sCmd.addCommand("rgboff",    rgbOff);
  ws2812fx.init();
  ws2812fx.setMode(ModeRGB.toInt()); // Режим
  uint32_t  tmp = strtol(("0x" + colorRGB).c_str(), NULL, 0);
  if (tmp >= 0x000000 && tmp <= 0xFFFFFF) {
    ws2812fx.setColor(tmp);
  }
  ws2812fx.setSpeed(speedRGB.toInt()); // Скорость
  ws2812fx.setBrightness(BrightnessRGB.toInt()); //Яркость
  //регистрируем модуль
  modulesReg("rgb");
}

// Задать режимы ленты
void handle_RGB() {
  //Получаем цвет
  colorRGB = HTTP.arg("c");
  if (colorRGB != "") {
    uint32_t  tmp = strtol(("0x" + colorRGB).c_str(), NULL, 0);
    if (tmp >= 0x000000 && tmp <= 0xFFFFFF) {
      Serial.print("colorRGB=");
      Serial.println(colorRGB);
      ws2812fx.setColor(tmp);
    }
  }
  //Получаем скорость
  speedRGB = HTTP.arg("s");
  if (speedRGB != "") {
    ws2812fx.setSpeed(speedRGB.toInt());
    Serial.print("speedRGB=");
    Serial.println(speedRGB);
  }
  //Получаем яркость
  BrightnessRGB = HTTP.arg("b");
  if (BrightnessRGB != "") {
    ws2812fx.setBrightness(BrightnessRGB.toInt());
    Serial.print("BrightnessRGB=");
    Serial.println(BrightnessRGB);
  }
  //Получаем режим
  ModeRGB = HTTP.arg("m");
  if (ModeRGB != "") {
    ws2812fx.setMode(ModeRGB.toInt() % ws2812fx.getModeCount());
    Serial.print("ModeRGB=");
    Serial.println(ModeRGB);
  }
  //Получаем время таймера
  timeRGB = HTTP.arg("time");
  if (timeRGB != "") {
  }
  //Получаем время сигнала
  timeRGB = HTTP.arg("s");
  if (timeBUZ != "") {
  }
  command = "rgbon";
  String state = "";
  if (stateRGB) {
    state = "\{\"title\":\"\{\{LangOn\}\}\",\"class\":\"btn btn-block btn-lg btn-primary\"\}";
  }
  else {
    state = "\{\"title\":\"\{\{LangOff\}\}\",\"class\":\"btn btn-block btn-lg btn-info\"\}";
  }

  HTTP.send(200, "text/plain", state);
}


void rgbNot() {

  if (stateRGB) {
    ws2812fx.stop();
  }
  else {
    ws2812fx.stop();
    ws2812fx.start();
  }
  stateRGB = !stateRGB;
}

void rgbOn() {

  if (!stateRGB) {
    ws2812fx.start();
    stateRGB = !stateRGB;
  }

}

void rgbOff() {

  if (stateRGB) {
    ws2812fx.stop();
    stateRGB = !stateRGB;
  }

}


