void initRGB() {
  modulesReg("rgb");
  //HTTP.on("/set", LedRGB);
  HTTP.on("/set", handle_RGB);
  ws2812fx.init();
  ws2812fx.setMode(ModeRGB.toInt()); // Режим

    uint32_t  tmp = strtol(("0x" + colorRGB).c_str(), NULL, 0);
    if (tmp >= 0x000000 && tmp <= 0xFFFFFF) {
      Serial.print("colorRGB=");
      Serial.println(colorRGB);
      ws2812fx.setColor(tmp);
    }
  ws2812fx.setSpeed(speedRGB.toInt()); // Скорость
  ws2812fx.setBrightness(BrightnessRGB.toInt()); //Яркость
  ws2812fx.start();
    ws2812fx.service();
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
    //ws2812fx.setBrightness(BrightnessRGB.toInt());
    Serial.print("timeRGB=");
    Serial.println(timeRGB);
  }
   //Получаем время сигнала
  timeRGB = HTTP.arg("s");
  if (timeBUZ != "") {
    //ws2812fx.setBrightness(BrightnessRGB.toInt());
    Serial.print("timeBUZ=");
    Serial.println(timeBUZ);
  }
  HTTP.send(200, "text/plain", "Ok");
}







