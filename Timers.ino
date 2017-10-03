void initTimers() {
  HTTP.on("/timerSave", HTTP_GET, [](AsyncWebServerRequest * request) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& Timers = jsonBuffer.parseObject(jsonTimer);
    JsonArray& arrays = Timers["timer"].asArray();
    JsonObject& record = arrays.createNestedObject();
    if (request->hasArg("id")) {
      record["id"]  = request->arg("id");
    }
    if (request->hasArg("trigger")) {
      record["trigger"]  = request->arg("trigger");
    }
    if (request->hasArg("module")) {
      record["module"]  = request->arg("module");
    }
    if (request->hasArg("day")) {
      record["day"]  = request->arg("day");
    }
    if (request->hasArg("time")) {
      record["time"]  = request->arg("time");
    }  if (request->hasArg("work")) {
      record["work"]  = request->arg("work");
    }
    jsonTimer = "";
    Timers.printTo(jsonTimer);
    writeFile("timer.save.json", jsonTimer );
    //loadTimer();
    request->send(200, "text/plain", responsTimer());
  });
  HTTP.on("/timersDel", HTTP_GET, [](AsyncWebServerRequest * request) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& Timers = jsonBuffer.parseObject(jsonTimer);
    JsonArray& nestedArray = Timers["timer"].asArray();
    //nestedArray.printTo(Serial);
    String id;
    if (request->hasArg("id")) {
      id = request->arg("id");
    }
    int y;
    for (int i = 0; i <= nestedArray.size() - 1; i++) {
      if (Timers["timer"][i]["id"] == id.toInt()) y = i;
    }
    nestedArray.removeAt(y);
    jsonTimer = "";
    Timers.printTo(jsonTimer);
    writeFile("timer.save.json", jsonTimer );
    //loadTimer();
    request->send(200, "text/plain", responsTimer());
  });

  HTTP.on("/timer.modules.json", HTTP_GET, [](AsyncWebServerRequest * request) {
    String responsA = "{\"content\":[";
  String responsB = "{}";
  String responsC = "{}";

  Serial.println(modules);
  responsC = jsonWrite(responsC, "rgb", "RGB");
  responsC = jsonWrite(responsC, "relay", "Relay");
  responsC = jsonWrite(responsC, "jalousie", "Jalousie");

  responsB = jsonWrite(responsB, "type", "select");
  responsB = jsonWrite(responsB, "name", "module");
  responsB = jsonWrite(responsB, "response", "[[trigger]]");
  responsB = jsonWrite(responsB, "action", "/trigger.[[module]].json");

  responsB = selectToMarker (responsB, "}");

  responsA += responsB;
  responsA += ",\"title\":";
  responsA += responsC;
  responsA += "}]}";
    request->send(200, "application/json", responsA);
  });



  // задача проверять таймеры каждую секунду.
  ts.add(0, 1000, [&](void*) {
    runTimers();
  }, nullptr, true);
  loadTimer();
  modulesReg("timers");
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

bool loadTimer() {
  Timerset = "";
  jsonTimer = readFile("timer.save.json", 4096);
  String Weekday = GetWeekday();
  configJson = jsonWrite(configJson, "Weekday", Weekday);
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
    Serial.println(Timerset);
  }
  //runTimers();
  return true;
}
void runTimers() {
  // Список текущих таймеров во временную переменную
  String timers = Timerset;
  String now = GetTime();
  String Weekday = GetWeekday();
  if (jsonRead(configJson, "Weekday") != Weekday) {
    configJson = jsonWrite(configJson, "Weekday", Weekday);
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
        Serial.println(command);

      }
      timers = timers.substring(timers.indexOf("\r\n") + 2);
    }
  } while (i != -1);
}

