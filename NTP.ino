#include <time.h>               //Содержится в пакете
void initNTP() {
  HTTP.on("/Time", handle_Time);     // Синхронизировать время устройства по запросу вида /Time
  HTTP.on("/timeZone", handle_time_zone);    // Установка времянной зоны
  timeSynch(jsonReadtoInt(configJson, "timeZone"));
  modulesReg("ntp");
}

void timeSynch(int zone) {
  if (WiFi.status() == WL_CONNECTED) {
    // Инициализация UDP соединения с NTP сервером
    configTime(zone * 3600, 0, "pool.ntp.org", "ru.pool.ntp.org");
    int i = 0;
    //Serial.print("\nWaiting for time ");
    while (!time(nullptr) && i < 10) {
      //Serial.print(".");
      i++;
      delay(100);
    }
    Serial.println("");
    configJson = jsonWrite(configJson, "time",  GetTime());
  }
}

// ---------- Синхронизация времени
void handle_Time() {
  timeSynch(jsonReadtoInt(configJson, "timeZone"));
  String out = "{}";
  out = jsonWrite(out, "title",   "{{LangTime1}} <strong id=time>"+GetTime()+"</strong>");
  HTTP.send(200, "text/plain", out); // отправляем ответ о выполнении
}

// ---------- Установка параметров времянной зоны по запросу вида http://192.168.0.101/timeZone?timezone=3
void handle_time_zone() {
  int timezone = HTTP.arg("timeZone").toInt(); // Получаем значение timezone из запроса конвертируем в int сохраняем в глобальной переменной
  configJson = jsonWrite(configJson, "timeZone",  timezone);
  timeSynch(timezone);
  writeFile("config.save.json", configJson );
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
  Data.replace("\n", "");
  int i = Data.lastIndexOf(" "); //Ишем позицию последнего символа пробел
  String Time = Data.substring(i - 8, i+1); // Выделяем время и пробел
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
