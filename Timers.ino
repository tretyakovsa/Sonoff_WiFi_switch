void timers_init() {
  HTTP.on("/timerSave", handle_timer_Save);
  HTTP.on("/timersDel", handle_timer_Del);
  loadTimer();
  modulesReg("timers");
}
void handle_timer_Save() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& Timers = jsonBuffer.parseObject(jsonTimer);
  JsonArray& arrays = Timers["timer"].asArray();
  JsonObject& record = arrays.createNestedObject();
  record["id"]  = HTTP.arg("id").toInt();
  record["trigger"]  = HTTP.arg("trigger");
  record["module"]  = HTTP.arg("module");
  record["day"]  = HTTP.arg("day");
  record["time"]  = HTTP.arg("time");
  record["work"]  = HTTP.arg("work").toInt();
  File TimersFile = SPIFFS.open("/timer.save.json", "w");
  if (!TimersFile) {
    HTTP.send(200, "text/plain", "Failed to open config file for writing");
    return;
  }
  Timers.printTo(TimersFile);
  TimersFile.close();
  loadTimer();
  HTTP.send(200, "text/plain", "OK");
}

void timer_Save() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& Timers = jsonBuffer.parseObject(jsonTimer);
   File TimersFile = SPIFFS.open("/timer.save.json", "w");
  if (!TimersFile) {
    Serial.println("Failed to open config file for writing");
    return;
  }
  Timers.printTo(TimersFile);
  TimersFile.close();
  loadTimer();
  }

void handle_timer_Del() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& Timers = jsonBuffer.parseObject(jsonTimer);
  JsonArray& nestedArray = Timers["timer"].asArray();
  //nestedArray.printTo(Serial);
  int y;
  for (int i = 0; i <= nestedArray.size() - 1; i++) {
    if (Timers["timer"][i]["id"] == HTTP.arg("id").toInt()) y = i;

  }
  nestedArray.removeAt(y);
  File TimersFile = SPIFFS.open("/timer.save.json", "w");
  if (!TimersFile) {
    HTTP.send(200, "text/plain", "Failed to open config file for writing");
    return;
  }
  Timers.printTo(TimersFile);
  TimersFile.close();
  loadTimer();
  HTTP.send(200, "text/plain", "OK");
}

bool loadTimer() {
  File TimersFile = SPIFFS.open("/timer.save.json", "r");
  if (!TimersFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = TimersFile.size();
  Weekday = GetWeekday();
  Timerset = "";
  if (size > 1024) {
    Serial.println("Config file size is too large");
    TimersFile.close();
    return false;
  }
  // загружаем файл конфигурации в глобальную переменную
  jsonTimer = TimersFile.readString();
  TimersFile.close();
  DynamicJsonBuffer jsonBuffer;
  JsonObject& Timers = jsonBuffer.parseObject(jsonTimer);
  JsonArray& nestedArray = Timers["timer"].asArray();
  int j = nestedArray.size();
  if (j != 0) {
    for (int i = 0; i <= j - 1; i++) {
      if ((Weekday == Timers["timer"][i]["day"].as<String>()) || (Timers["timer"][i]["day"].as<String>() == "All")) {
        Timerset += Timers["timer"][i]["time"].as<String>() + ",";
        Timerset += Timers["timer"][i]["module"].as<String>() + ",";
        Timerset += Timers["timer"][i]["work"].as<String>() + ",";
        Timerset += Timers["timer"][i]["trigger"].as<String>();
        Timerset += "\r\n";
      }
    }
    //Serial.println(Timerset);
  }
  //runTimers();
  return true;
}

void runTimers() {
  // Список текущих таймеров во временную переменную
  String timers = Timerset;
  int i;
  // Будем повторять проверку для каждого установленного таймера
  do {
    // проверяем есть ли таймеры
    i = timers.indexOf("\r\n");
       if (i != -1) {
      // получаем строку текщего таймера
      String timer = timers.substring(0, i);
      // Если время совпадает с текущим
        if (timer.substring(0, 8) == Time) {
        int j = timer.lastIndexOf(",");
        // Загружаем время работы реле
        timeSonoff = timer.substring(9, j).toInt();
        // выполняем необходимое действие
        Serial.println(timer.substring(j + 1));
        String temp = timer.substring(j + 1);
        if (temp == "not") {
          task=1;
          chaing = 1;
        }
        if (temp == "on") {
          task=1;
          state0 = 0;
          chaing = 1;
        }
        if (temp == "off") {
          task=1;
          state0 = 1;
          chaing = 1;
        }
      }
      timers = timers.substring(timers.indexOf("\r\n") + 2);
      //Serial.println(timers);
    }
  } while (i != -1);

}
