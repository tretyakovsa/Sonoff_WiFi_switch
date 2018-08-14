#include <time.h>               //Содержится в пакете
void initNTP() {
  String ntpTemp = readArgsString();
  sendOptions(ntp1S, ntpTemp);
  ntpTemp = readArgsString();
  sendOptions(ntp2S, ntpTemp);
  sCmd.addCommand("time", handle_time);
  timeSynch();
  //Serial.println(GetTime());
  if (GetTime()!="00:00:00"){
  // задача проверять таймеры каждую секунду обновлять текущее время.
  ts.add(0, 1000, [&](void*) {
    String timeNow = GetTime();
    if (timeNow == "00:00:00") {
      String timeNow = GetWeekday();
      sendStatus(weekdayS, timeNow);
      timeSynch();
      loadTimer();
    }
    sendStatus(timeS, timeNow);
    sendOptions(timeS, timeNow);
    if (timeNow == minTime) {
      sCmd.readStr(comTime);
      delTimer();
      loadTimer();
    }
    if (timeNow == pTime) {
      sCmd.readStr(comTimeP);
      comTimeP ="";
      pTime ="";
    }
  }, nullptr, true);
  sCmd.addCommand("zone", handle_timeZone);
  modulesReg("ntp");
  }
}

// ------------------------------ Установка времянной зоны
void handle_timeZone() {
  int timezone = readArgsInt();
  sendSetup(timeZoneS,  timezone);
  sendOptions(timeZoneS, timezone);
  timeSynch();
  saveConfigSetup ();
}

// ------------------------------ Комманда синхронизации времени
void handle_time() {
     timeSynch();
     statusS = "{}";
    jsonWrite(statusS, "title",   "{{LangTime1}} <strong id=time>" + GetTime() + "</strong>");
}

void timeSynch() {
  uint8_t zone = getOptionsInt(timeZoneS);
  String ntp1 = getOptions(ntp1S);
  String ntp2 = getOptions(ntp2S);
   if (ntp1 == emptyS) ntp1 = "pool.ntp.org";
   if (ntp2 == emptyS) ntp2 = "ru.pool.ntp.org";
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
    timeNow = GetWeekday();
    sendStatus(weekdayS, timeNow);
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



