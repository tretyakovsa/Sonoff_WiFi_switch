#include <time.h>               //Содержится в пакете
void ntp_init() {
  HTTP.on("/Time", handle_Time);     // Синхронизировать время устройства по запросу вида /Time
  HTTP.on("/timeZone", handle_time_zone);    // Установка времянной зоны
  timeSynch(timezone);
  modulesReg("ntp");
}
void timeSynch(int zone) {
  if (WiFi.status() == WL_CONNECTED) {
    // Инициализация UDP соединения с NTP сервером
    configTime(zone * 3600, 0, "pool.ntp.org", "ru.pool.ntp.org");
    int i = 0;
    Serial.println("\nWaiting for time");
    while (!time(nullptr) && i < 10) {
      Serial.print(".");
      i++;
      delay(1000);
    }
    Serial.println("");
    Serial.println("ITime Ready!");
    Serial.println(GetTime());
    Serial.println(GetDate());
  }
}

void handle_Time() {
  timeSynch(timezone);
  Serial.println(timezone);
  HTTP.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
}
// Установка параметров времянной зоны по запросу вида http://192.168.0.101/TimeZone?timezone=3
void handle_time_zone() {
  timezone = HTTP.arg("timeZone").toInt(); // Получаем значение timezone из запроса конвертируем в int сохраняем в глобальной переменной
  timeSynch(timezone);
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

// Получение текущего времени
String GetTime() {
  time_t now = time(nullptr); // получаем время с помощью библиотеки time.h
  String Time = ""; // Строка для результатов времени
  Time += ctime(&now); // Преобразуем время в строку формата Thu Jan 19 00:55:35 2017
  int i = Time.indexOf(":"); //Ишем позицию первого символа :
  Time = Time.substring(i - 2, i + 6); // Выделяем из строки 2 символа перед символом : и 6 символов после

  return Time; // Возврашаем полученное время
}


// Получение даты
String GetDate() {
  time_t now = time(nullptr); // получаем время с помощью библиотеки time.h
  String Data = ""; // Строка для результатов времени
  Data += ctime(&now); // Преобразуем время в строку формата Thu Jan 19 00:55:35 2017
  int i = Data.lastIndexOf(" "); //Ишем позицию последнего символа пробел
  String Time = Data.substring(i - 8, i + 1); // Выделяем время и пробел
  Data.replace(Time, ""); // Удаляем из строки 8 символов времени и пробел
  return Data; // Возврашаем полученную дату
}
// Получение дня недели
String GetWeekday() {
  String Data = GetDate();
  int i = Data.indexOf(" "); //Ишем позицию первого символа пробел
  String weekday = Data.substring(i - 3, i); // Выделяем время и пробел
  return weekday;
}

// Вызывается каждую секунду в обход основного цикла.
void sectest() {
  Time = GetTime();
  runTimers();

  // Калибровка времени каждые сутки, получение текушего дня недели
 if (calibrationTime.compareTo(Time) == 0) {
  //task=1;
 }
 if (pirTime > 0 && state0 == 0 && digitalRead(PIR_PIN)) {
  //alarm_pir();
 }

 String ddnsTime = Time.substring(3, 8); // Выделяем из строки минуты секунды
 //Serial.println(ddnsTime);
 // В 15, 30, 45 минут каждого часа идет запрос на сервер ddns
 if ((ddnsTime == "00:00" || ddnsTime == "15:00" || ddnsTime == "30:00"|| ddnsTime == "45:00")) {
  Serial.println("15 minits");
  ddnsTest = true;
 }
}

