void tachinit() {
  // Кнопка будет работать по прерыванию
  attachInterrupt(TACH_PIN, Tach_0, RISING); //прерывание сработает, когда состояние вывода изменится с низкого уровня на высокий

  // Виртуальная кнопка будет реагировать на
  HTTP.on("/sonoff", handle_sonoff);
  HTTP.on("/relay", handle_sonoff);
  HTTP.on("/relayon", handle_relayon);
  HTTP.on("/relayoff", handle_relayoff);

  HTTP.on("/rgb", LedRGB);
}
void handle_sonoff() {
  command = "relaynot";
  String state = "";
  if (state0){state = "\{\"title\":\"\{\{LangOn\}\}\",\"class\":\"btn btn-block btn-lg btn-primary\"\}"; }
  else {state = "\{\"title\":\"\{\{LangOff\}\}\",\"class\":\"btn btn-block btn-lg btn-info\"\}"; }

  HTTP.send(200, "text/json", state);
}
void handle_relayon() {
  command = "relayon";
   String state = "";
  if (state0){state = "\{\"title\":\"\{\{LangOn\}\}\",\"class\":\"btn btn-block btn-lg btn-primary\"\}"; }
  else {state = "\{\"title\":\"\{\{LangOff\}\}\",\"class\":\"btn btn-block btn-lg btn-info\"\}"; }
  HTTP.send(200, "text/json", state);
}
void handle_relayoff() {
  command = "relayoff";
String state = "";
  if (state0){state = "\{\"title\":\"\{\{LangOn\}\}\",\"class\":\"btn btn-block btn-lg btn-primary\"\}"; }
  else {state = "\{\"title\":\"\{\{LangOff\}\}\",\"class\":\"btn btn-block btn-lg btn-info\"\}"; }
  HTTP.send(200, "text/json", state);
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
    //command = "rgbnot";
    command = "relaynot";
  }
  millis_prev = millis();
}

