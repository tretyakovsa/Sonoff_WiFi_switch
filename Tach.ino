void tachinit(){
 // Кнопка будет работать по прерыванию
 attachInterrupt(TACH_PIN, Tach_0, RISING); //прерывание сработает, когда состояние вывода изменится с низкого уровня на высокий
  }
// Выполняется при нажатии на кнопку
void Tach_0() {
 static unsigned long millis_prev;
 // Устроняем дребезг контакта
 if (millis() - 100 > millis_prev) {
  task = 1;
  chaing = 1; // Выстовляем признак нажатия кнопки
 }
 millis_prev = millis();
}


