// Выполняется при нажатии на кнопку
void Tach_0() {
 static unsigned long millis_prev;
 // Устроняем дребезг контакта
 if (millis() - 100 > millis_prev) {
  chaing = 1; // Выстовляем признак нажатия кнопки
 }
 millis_prev = millis();

}


void Time01() {
  tickerSetLow.attach(TimeLed, setT1, 0);
  chaing = 1;

}

void Time02() {
  tickerSetLow.attach(TimeLed, setT2, 0);
  Serial.println(TimeLed);
  Serial.println("Time2");
  chaing = 1;

}

void setT1(int state) {
 tickerSetLow.detach();
 Serial.println("Time1-OFF");
 chaing = 1;

}

void setT2(int state) {
 tickerSetLow.detach();
 Serial.println("Time2-OFF");
 chaing = 1;

}

