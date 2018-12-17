void initTimers() {
  loadTimer();
  modulesReg("timers");
}
void delTimer() {
  String jsonTimer = readFile(configTimerS, 4096);
  DynamicJsonBuffer jsonBuffer;
  JsonObject& Timers = jsonBuffer.parseObject(jsonTimer);
  JsonArray& nestedArray = Timers["timer"].asArray();
  uint8_t y=255;
  for (int i = 0; i <= nestedArray.size() - 1; i++) {
    if (Timers["timer"][i]["id"] == idTime) y = i;
  }
  if (y!=255){
  nestedArray.removeAt(y);
  jsonTimer = emptyS;
  Timers.printTo(jsonTimer);
  writeFile(configTimerS, jsonTimer);
  //Serial.println(jsonTimer);
  }
}

void loadTimer() {
  //Serial.println("Load");
  long lminTime = 86400;
  String jsonTimer = readFile(configTimerS, 4096);
  String Weekday = GetWeekday();
  comTime = emptyS;
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
        String idTimes = Timers["timer"][i]["id"].as<String>();
        if (idTimes.indexOf(":")==-1) idTimes=emptyS;
        String nextTime1 = Timers["timer"][i]["time1"].as<String>();
        String nextcom1 = Timers["timer"][i]["com1"].as<String>();
        String nextRun1 = Timers["timer"][i]["run1"].as<String>();
        String nextId = Timers["timer"][i]["id"].as<String>();
        int active = Timers["timer"][i]["active"];
        if (timeToLong(nextTime1) >= timeToLong(GetTime()) && !active) {
          if (lminTime >= timeToLong(nextTime1)) {
            lminTime = timeToLong(nextTime1);
            minTime = nextTime1;
            comTime = nextcom1;
            runTime = nextRun1;
            idTime = nextId;
          }
        }
        sendOptions("timersT", minTime);
        sendOptions("timersC", comTime);
        sendOptions("timersR", runTime);
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

int  timeToMin(String Time) {
  //"00:00:00"  время в секунды
  long  min = selectToMarker(Time, ":").toInt() * 60; //общее количество секунд в полных часах
  Time = deleteBeforeDelimiter (Time, ":"); // Теперь здесь минуты секунды
  min += selectToMarker(Time, ":").toInt(); // Добавим секунды из полных минут
  return min;
}

String timeToString(long Time) {
  String str;
  uint8_t temp = (Time / 3600);
  if (temp < 10) str += "0";
  str += temp; // здесь часы
  str += ":";
  Time = (Time - temp * 3600); // отбросим часы
  temp = (Time / 60);
  if (temp < 10) str += "0";
  str += temp; // здесь минуты
  str += ":";
  Time = (Time - temp * 60); // отбросим минуты здесь секунды
  if (Time < 10) str += "0";
  str += Time;
  return str;
}
