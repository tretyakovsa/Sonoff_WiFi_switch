void initTimers() {
  HTTP.on("/timerSave", handle_timer_Save);
  HTTP.on("/timersDel", handle_timer_Del);
  HTTP.on("/timer.modules.json", handle_timer_Mod);

  // задача проверять таймеры каждую секунду.
  ts.add(0, 1000, [&](void*) {
    runTimers();
  }, nullptr, true);
  loadTimer();
  modulesReg("timers");
}

void handle_timer_Mod(){
  String responsA = "{\"content\":[";
  String responsB ="{}";
  String responsC ="{}";

responsC = jsonWrite(responsC, "rgb", "RGB");
responsC = jsonWrite(responsC, "relay", "Relay");
responsC = jsonWrite(responsC, "jalousie", "Jalousie");

   responsB = jsonWrite(responsB, "type", "select");
   responsB = jsonWrite(responsB, "name", "module");
   responsB = jsonWrite(responsB, "response", "[[trigger]]");
   responsB = jsonWrite(responsB, "action", "/trigger.[[module]].json");

   responsB = selectToMarker (responsB, "}");

   responsA +=responsB;
   responsA += ",\"title\":";
   responsA +=responsC;
   responsA += "}]}";
   HTTP.send(200, "application/json", responsA);
  }


void handle_timer_Save() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& Timers = jsonBuffer.parseObject(jsonTimer);
  JsonArray& arrays = Timers["timer"].asArray();
  JsonObject& record = arrays.createNestedObject();
  record["id"]  = HTTP.arg("id");
  record["trigger"]  = HTTP.arg("trigger");
  record["module"]  = HTTP.arg("module");
  record["day"]  = HTTP.arg("day");
  record["time"]  = HTTP.arg("time");
  record["work"]  = HTTP.arg("work");
  jsonTimer = "";
  Timers.printTo(jsonTimer);
  writeFile("timer.save.json", jsonTimer );

  loadTimer();
  HTTP.send(200, "text/plain", responsTimer());
}

String responsTimer() {
  String responsA = "{\"state\": \"timer.save.json\",\"title\":";
  String responsB = "{}";
  responsB = jsonWrite(responsB, "module", "");
  responsB = jsonWrite(responsB, "trigger", "{{LangOn.}}/{{LangOff.}}");
  responsB = jsonWrite(responsB, "day", "{{LangDay}}");
  responsB = jsonWrite(responsB, "time", "{{LangTime4}}");
  responsB = jsonWrite(responsB, "work", "{{LangWorkTime}}");
  responsB = jsonWrite(responsB, "id", "{{LangDel}}");
  return responsA += responsB += "}";
}


void handle_timer_Del() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& Timers = jsonBuffer.parseObject(jsonTimer);
  JsonArray& nestedArray = Timers["timer"].asArray();
  int y;
  for (int i = 0; i <= nestedArray.size() - 1; i++) {
    if (Timers["timer"][i]["id"] == HTTP.arg("id").toInt()) y = i;

  }
  nestedArray.removeAt(y);
  jsonTimer = "";
  Timers.printTo(jsonTimer);
  writeFile("timer.save.json", jsonTimer );
  loadTimer();
  HTTP.send(200, "text/plain", responsTimer());
}


bool loadTimer() {
  Timerset = "";
  jsonTimer = readFile("timer.save.json", 4096);
  String Weekday = GetWeekday();
  configJson = jsonWrite(configJson, "weekday", Weekday);
  DynamicJsonBuffer jsonBuffer;
  JsonObject& Timers = jsonBuffer.parseObject(jsonTimer);
  JsonArray& nestedArray = Timers["timer"].asArray();
  int j = nestedArray.size();
  if (j != 0) {
    for (int i = 0; i <= j - 1; i++) {
      if ((Weekday == Timers["timer"][i]["day"].as<String>()) || (Timers["timer"][i]["day"].as<String>() == "All")) {

        Timerset += Timers["timer"][i]["time"].as<String>() + ",";
        Timerset += Timers["timer"][i]["module"].as<String>() + ",";
        Timerset += Timers["timer"][i]["trigger"].as<String>() + ",";
        Timerset += Timers["timer"][i]["work"].as<String>() + ",";
        Timerset += Timers["timer"][i]["id"].as<String>();
        Timerset += "\r\n";
      }
    }
  }
  //runTimers();
  return true;
}
void runTimers() {
  // Список текущих таймеров во временную переменную
  String timers = Timerset;
  String now = GetTime();
  String Weekday = GetWeekday();
  if (jsonRead(configJson, "weekday") != Weekday) {
    configJson = jsonWrite(configJson, "weekday", Weekday);
    loadTimer();
  }
  configSetup = jsonWrite(configSetup, "time", now);
  configJson = jsonWrite(configJson, "time", now);
  int i;
  // Будем повторять проверку для каждого установленного таймера
  do {
    // проверяем есть ли таймеры
    i = timers.indexOf("\r\n");
    if (i != -1) {
      // получаем строку текщего таймера
      String timer = timers.substring(0, i);
      if (timer.substring(0, 8) == now) {
        // Отрезаем время из строки 12:44:15,relay,not,work,id
        timer = deleteBeforeDelimiter(timer, ",");
        // Выделяем модуль
        String module = selectToMarker (timer, ",");
        // отрезаем модуль
        timer = deleteBeforeDelimiter(timer, ",");
        // Выделяем команду
        String com = selectToMarker (timer, ",");
        // отрезаем команду
        timer = deleteBeforeDelimiter(timer, ",");
        // Выделяем интервал таймера
        String interval = selectToMarker (timer, ",");
        // отрезаем интервал
        timer = deleteBeforeDelimiter(timer, ",");
        // выделяем id таймера
        String id = selectToMarker (timer, "\r\n");
        // выполняем необходимое действие

        command = module + com + " " + interval + " " + id;

      }
      timers = timers.substring(timers.indexOf("\r\n") + 2);
    }
  } while (i != -1);
}

