#include <time.h>               //Содержится в пакете
void Time_init() {
  HTTP.on("/Time", handle_Time);     // Синхронизировать время устройства по запросу вида /Time
  HTTP.on("/timeZone", handle_time_zone);    // Установка времянной зоны
  HTTP.on("/timerSave", handle_timer_Save);
  HTTP.on("/timersDel", handle_timer_Del);
  timeSynch(timezone);
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
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

void handle_timer_Save() {
  loadTimer();
  DynamicJsonBuffer jsonBuffer;
  JsonObject& Timers = jsonBuffer.parseObject(jsonTimer);
  JsonArray& arrays = Timers["timer"].asArray();
  JsonObject& record = arrays.createNestedObject();
  record["id"]  = HTTP.arg("id").toInt();
  record["trigger"]  = HTTP.arg("trigger");
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
  HTTP.send(200, "text/plain", "OK");
}

void handle_timer_Del() {
  loadTimer();
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

String GetWeekday() {
  String Data = GetDate();
  int i = Data.indexOf(" "); //Ишем позицию первого символа пробел
  String weekday = Data.substring(i - 3, i + 1); // Выделяем время и пробел
  return weekday;
}

bool loadTimer() {
  File TimersFile = SPIFFS.open("/timer.save.json", "r");
  if (!TimersFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = TimersFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    TimersFile.close();
    return false;
  }
  // загружаем файл конфигурации в глобальную переменную
  jsonTimer = TimersFile.readString();
  DynamicJsonBuffer jsonBuffer;
  JsonObject& Timers = jsonBuffer.parseObject(jsonTimer);
  JsonArray& nestedArray = Timers["timer"].asArray();
  for (int i = 0; i <= nestedArray.size() - 1; i++) {
    //Serial.println(Timers["timer"][i]["time"].as<String>());
  }
  TimersFile.close();
  return true;
}
