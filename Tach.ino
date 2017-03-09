void tachinit(){
 // Кнопка будет работать по прерыванию
 attachInterrupt(TACH_PIN, Tach_0, RISING); //прерывание сработает, когда состояние вывода изменится с низкого уровня на высокий
  }
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
  tickerSetLow.attach(timeSonoff*60, setT1, 0);
  chaing = 1;
}

void alarm_pir() {
  tickerSetLow.attach(pirTime, setT1, 0);
  state0 = 0;
  chaing = 1;
}

void setT1(int state) {
 tickerSetLow.detach();
 chaing = 1;
}
