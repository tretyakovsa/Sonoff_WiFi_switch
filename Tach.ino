void tachinit() {
  // Кнопка будет работать по прерыванию
  attachInterrupt(TACH_PIN, Tach_0, RISING); //прерывание сработает, когда состояние вывода изменится с низкого уровня на высокий

  // Виртуальная кнопка будет реагировать на
  HTTP.on("/sonoff", handle_sonoff);
  HTTP.on("/relay", handle_sonoff);

  HTTP.on("/rgb", LedRGB);
}
void handle_sonoff() {
  command = "relaynot";
  HTTP.send(200, "text/json", "Ok");
}

void LedRGB() {
  command = "rgbnot";
  HTTP.send(200, "text/json", "Ok");
}

// Выполняется при нажатии на кнопку
void Tach_0() {
  static unsigned long millis_prev;
  // Устроняем дребезг контакта
  if (millis() - 100 > millis_prev) {
    command = "relaynot";
  }
  millis_prev = millis();
}

