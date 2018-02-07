#include <time.h>               //Содержится в пакете
void initNTP() {
  String ntpTemp = readArgsString();
  if (ntpTemp == "") ntpTemp = "pool.ntp.org";
  sendOptions(ntp1S, ntpTemp);
  ntpTemp = readArgsString();
  if (ntpTemp == "") ntpTemp = "ru.pool.ntp.org";
  sendOptions(ntp2S, ntpTemp);
  HTTP.on("/Time", HTTP_GET, []() {
    timeSynch(getOptionsInt(timeZoneS),getOptions(ntp1S),getOptions(ntp2S));
    String out = "{}";
    out = jsonWrite(out, "title",   "{{LangTime1}} <strong id=time>" + GetTime() + "</strong>");
    httpOkText(out);
  });
  // Установка времянной зоны
  HTTP.on("/timeZone", HTTP_GET, []() {
    uint8_t timezone = HTTP.arg("timeZone").toInt();
    jsonWrite(configSetup, timeZoneS,  timezone);
    sendOptions(timeZoneS, timezone);
    timeSynch(getOptionsInt(timeZoneS),getOptions(ntp1S),getOptions(ntp2S));
    saveConfigSetup ();
    httpOkText();
  });
  timeSynch(getOptionsInt(timeZoneS),getOptions(ntp1S),getOptions(ntp2S));
    // задача проверять таймеры каждую секунду обновлять текущее время.
  ts.add(0, 1000, [&](void*) {
    String timeNow = GetTime();
    jsonWrite(configSetup, timeS,  timeNow);
    sendStatus(timeS, timeNow);
    sendOptions(timeS, timeNow);
    flagT = true;
  }, nullptr, true);
  modulesReg("ntp");
}

void timeSynch(uint8_t zone, String ntp1, String ntp2) {
  if (WiFi.status() == WL_CONNECTED) {
    // Инициализация UDP соединения с NTP сервером
    configTime(zone * 3600, 0, ntp1.c_str(), ntp2.c_str());
    uint8_t i = 0;
    while (!time(nullptr) && i < 10) {
      i++;
      delay(100);
    }
    String timeNow = GetTime();
    jsonWrite(configSetup, timeS,  timeNow);
    sendStatus(timeS, timeNow);
    //sendOptions(timeS, timeNow);
    timeNow = GetWeekday();
    //configSetup = jsonWrite(configSetup, weekdayS,  timeNow);
    sendStatus(weekdayS, timeNow);
    //sendOptions(timeS, timeNow);
  }
}
// Получение текущего времени
String GetTime() {
  time_t now = time(nullptr); // получаем время с помощью библиотеки time.h
  String Time = ""; // Строка для результатов времени
  Time += ctime(&now); // Преобразуем время в строку формата Thu Jan 19 00:55:35 2017
  uint8_t i = Time.indexOf(":"); //Ишем позицию первого символа :
  Time = Time.substring(i - 2, i + 6); // Выделяем из строки 2 символа перед символом : и 6 символов после
  return Time; // Возврашаем полученное время
}

// Получение даты
String GetDate() {
  time_t now = time(nullptr); // получаем время с помощью библиотеки time.h
  String Data = ""; // Строка для результатов времени
  Data += ctime(&now); // Преобразуем время в строку формата Thu Jan 19 00:55:35 2017
  Data.replace("\n", "");
  uint8_t i = Data.lastIndexOf(" "); //Ишем позицию последнего символа пробел
  String Time = Data.substring(i - 8, i + 1); // Выделяем время и пробел
  Data.replace(Time, ""); // Удаляем из строки 8 символов времени и пробел
  return Data; // Возврашаем полученную дату
}
// Получение дня недели
String GetWeekday() {
  String Data = GetDate();
  uint8_t i = Data.indexOf(" "); //Ишем позицию первого символа пробел
  String weekday = Data.substring(i - 3, i); // Выделяем время и пробел
  return weekday;
}
