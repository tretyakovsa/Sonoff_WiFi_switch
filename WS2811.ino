void initRGB() {
  //HTTP.on("/set", LedRGB);
  HTTP.on("/set", handle_RGB);
  //modes.reserve(5000);
  ws2812fx.init();
  ws2812fx.setMode(DEFAULT_MODE); // Режим
  ws2812fx.setColor(DEFAULT_COLOR); // Цвет
  ws2812fx.setSpeed(DEFAULT_SPEED); // Скорость
  ws2812fx.setBrightness(DEFAULT_BRIGHTNESS); //Яркость
  ws2812fx.start();
}

// Задать режимы ленты
void handle_RGB() {
  //Получаем цвет
  color = HTTP.arg("c");
  uint32_t  tmp = strtol(("0x"+color).c_str(),NULL,0);
if (tmp >= 0x000000 && tmp <= 0xFFFFFF) {
      ws2812fx.setColor(tmp);
     }
ws2812fx.setSpeed(HTTP.arg("s").toInt());
ws2812fx.setBrightness(HTTP.arg("b").toInt());
     //Получаем режим
 // color = HTTP.arg("m");
 // uint32_t  tmp = strtol(("0x"+color).c_str(),NULL,0);
 // uint8_t tmp = (uint8_t) strtol(&HTTP.arg(i)[0], NULL, 10);

   // String m = HTTP.arg("m");

    //  uint8_t tmp = strtol(m.c_str(),NULL,0);
      ws2812fx.setMode(HTTP.arg("m").toInt() % ws2812fx.getModeCount());



  HTTP.send(200, "text/plain", color);
}







