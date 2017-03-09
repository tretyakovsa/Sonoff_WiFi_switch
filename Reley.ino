void reley_init() {
  pinMode(RELE1_PIN, OUTPUT);
  HTTP.on("/sonoff", sonoffActiv);                // задать цвет ленты и включить.
  HTTP.on("/reley", releyActiv);                // запуск мотора напровление храниться в переменной
}

// Меняет флаг для включения выключения sonoff
void sonoffActiv() {
  chaing = 1;
  HTTP.send(200, "text/plain", "OK");
}

// Меняет флаг для запуска
void releyActiv() {
  Serial.println(state0);
  chaing = 1;
  HTTP.send(200, "text/plain", "OK");
}
