void initTimers() {
  loadTimer();
}

void loadTimer() {
  long lminTime = 86400;
  String jsonTimer = readFile(configTimerS, 4096);
  String Weekday = GetWeekday();
  comTime = "";
  //  Получаем текущий день недели в виде числа
  uint8_t iDay = 7;
  if (Weekday == "Sun") iDay = 0;
  if (Weekday == "Mon") iDay = 1;
  if (Weekday == "Tue") iDay = 2;
  if (Weekday == "Wed") iDay = 3;
  if (Weekday == "Thu") iDay = 4;
  if (Weekday == "Fri") iDay = 5;
  if (Weekday == "Sat") iDay = 6;
  // Приготовились читать массив таймеров
  DynamicJsonBuffer jsonBuffer;
  JsonObject& Timers = jsonBuffer.parseObject(jsonTimer);
  JsonArray& nestedArray = Timers["timer"].asArray();
  // Сколько тамеров в массиве
  int j = nestedArray.size();
  // Если есть таймеры
  if (j != 0) {
    // Перебор всех по порядку
    for (int i = 0; i <= j - 1; i++) {
      // Возьмем только те которые соответствуют текущему дню недели
      String week = Timers["timer"][i]["day"].as<String>(); // признак принодлежности к дню недели
      int ind = week.substring(iDay, iDay + 1).toInt(); // Выделяем нужный день недели
      if (ind) { // Если день недели совпадает
        String nextTime1 = Timers["timer"][i]["time1"].as<String>();
        String nextTime2 = Timers["timer"][i]["time2"].as<String>();
        String nextcom1 = Timers["timer"][i]["com1"].as<String>();
        String nextcom2 = Timers["timer"][i]["com2"].as<String>();
        if (timeToLong(nextTime1) >= timeToLong(GetTime())) {
          if (lminTime >= timeToLong(nextTime1)) {
            lminTime = timeToLong(nextTime1);
            minTime = nextTime1;
            comTime = nextcom1;
          }
        }
        if (nextTime2 != "") {
          if (timeToLong(nextTime2) >= timeToLong(GetTime())) {
            if (lminTime >= timeToLong(nextTime2)) {
              lminTime = timeToLong(nextTime2);
              minTime = nextTime2;
              comTime = nextcom2;
            }
          }
        }
        sendOptions("timersT", minTime);
        sendOptions("timersC", comTime);
      }
    }
  }
  //Serial.println(minTime);
}

long  timeToLong(String Time) {
  //"00:00:00"  время в секунды
  long  second = selectToMarker(Time, ":").toInt() * 3600; //общее количество секунд в полных часах
  Time = deleteBeforeDelimiter (Time, ":"); // Теперь здесь минуты секунды
  second += selectToMarker(Time, ":").toInt() * 60; // Добавим секунды из полных минут
  Time = deleteBeforeDelimiter (Time, ":"); // Теперь здесь секунды
  second += selectToMarker(Time, ":").toInt(); // Добавим секунды
  return second;
}

String timeToString(long Time) {
  String str = "";
  str += String((Time / 3600)); // здесь часы
  str += ":";
  Time = (Time - (Time / 3600) * 3600); // отбросим часы
  str += String((Time / 60)); // здесь минуты
  str += ":";
  Time = (Time - (Time / 60) * 60); // отбросим минуты здесь секунды
  str += String((Time - (Time / 60) * 60));
  return str;
}
