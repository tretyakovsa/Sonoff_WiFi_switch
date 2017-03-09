void relay_init() {
  pinMode(RELE1_PIN, OUTPUT);
  HTTP.on("/sonoff", sonoffActiv);                // задать цвет ленты и включить.
  HTTP.on("/relay", relayActiv);                // запуск мотора напровление храниться в переменной
  modulesReg("relay");
}

// Меняет флаг для включения выключения sonoff
void sonoffActiv() {
  chaing = 1;
  HTTP.send(200, "text/plain", "OK");
}

// Меняет флаг для запуска
void relayActiv() {
  Serial.println(state0);
  chaing = 1;
  HTTP.send(200, "text/plain", "OK");
}
