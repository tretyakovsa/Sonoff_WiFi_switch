void initTimers() {
//  Serial.println();
  loadTimer();
  Serial.println(Timerset);

  //Serial.println(timeToLong("12:15:00"));
  //handleTimer();

}

void loadTimer() {
  long lminTime = 86400;
  jsonTimer = readFile(configTimerS, 4096);
  Timerset = "";
  String Weekday = GetWeekday();
  uint8_t iDay = 7;
  if (Weekday == "Sun") iDay = 0;
  if (Weekday == "Mon") iDay = 1;
  if (Weekday == "Tue") iDay = 2;
  if (Weekday == "Wed") iDay = 3;
  if (Weekday == "Thu") iDay = 4;
  if (Weekday == "Fri") iDay = 5;
  if (Weekday == "Sat") iDay = 6;
  DynamicJsonBuffer jsonBuffer;
  JsonObject& Timers = jsonBuffer.parseObject(jsonTimer);
  JsonArray& nestedArray = Timers["timer"].asArray();
  int j = nestedArray.size();
  if (j != 0) {
    for (int i = 0; i <= j - 1; i++) {
      String week = Timers["timer"][i]["day"].as<String>();
      int ind = week.substring(iDay, iDay + 1).toInt();
      if (ind != 7) {
        String nextTime = Timers["timer"][i]["time1"].as<String>();
        if (lminTime >= timeToLong(nextTime)) {
          lminTime = timeToLong(nextTime);
          //if (timeToLong(GetTime())<lminTime){
          minTime = nextTime;

        }
        Timerset += nextTime + "-";
        String temp = Timers["timer"][i]["time2"].as<String>();
        Timerset += Timers["timer"][i]["com1"].as<String>();
        if (temp != "") {
          if (lminTime >= timeToLong(temp)) {
            lminTime = timeToLong(temp);
            minTime = temp;
          }
          Timerset += "," + temp + "-";
          Timerset += Timers["timer"][i]["com2"].as<String>() + ",";
        }
        else Timerset += temp + ",";
      }
    }
  }
}

void handleTimer() {

  String timers = Timerset;
  String nextTimer;
  do {
    nextTimer = selectToMarker(timers, ","); // выделяем таймер
    //Serial.println(nextTimer);
    if (nextTimer != "") {
      String timeT = selectToMarker(nextTimer, "-");   // выделяем время
      if (timeT == minTime) {
        String com = deleteBeforeDelimiter(nextTimer, "-"); // выделяем комманду
//        Serial.println(timeT);
//        Serial.println(com);
      }
    }
    timers = deleteBeforeDelimiter(timers, ",");
  } while (nextTimer != "");
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
