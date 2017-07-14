// ---------------Инициализация модулей
void initCMD() {
  sCmd.addCommand("Serial",       uart);
  sCmd.addCommand("wifi",       initWIFI);
  sCmd.addCommand("NTP",        initNTP);
  sCmd.addCommand("Upgrade",    initUpgrade);
  sCmd.addCommand("SSDP",       initSSDP);
  sCmd.addCommand("HTTP",       initHTTP);
  sCmd.addCommand("DDNS",       initDDNS);
  sCmd.addCommand("A0",       initA0);
  sCmd.addCommand("Tach",       initTach);
  sCmd.addCommand("DHT",       initDHT);
  sCmd.addCommand("D18B20",       initD18B20);
  sCmd.addCommand("TIMERS",       initTimers);
  sCmd.addCommand("RELAY",       initRelay);
  sCmd.addCommand("MQTT",       initMQTT);
  sCmd.addCommand("RGB",       initRGB);
  sCmd.addCommand("RCSwitch",       initRCSwitch);
  sCmd.addCommand("MOTION",       initMotion);
  sCmd.setDefaultHandler(unrecognized);
}

void unrecognized(const char *command) {
  Serial.println("What?");
}

void uart(){
  Serial.end();
  Serial.begin(readArgsInt());
  delay(100);
  }

String readArgsString() {
  String arg;
  arg = sCmd.next();    // Get the next argument from the SerialCommand object buffer
  if (arg=="") arg="";
  return arg;
}

int readArgsInt() {
  char *arg;
  arg = sCmd.next();    // Get the next argument from the SerialCommand object buffer
  if (arg != NULL) {
    return atoi(arg);
  }
  else {
    return 0;
  }
}

// ------------- Чтение файла в строку
String readFile(String fileName, size_t len ) {
  File configFile = SPIFFS.open("/" + fileName, "r");
  if (!configFile) {
    return "Failed to open config file";
  }
  size_t size = configFile.size();
  if (size > len) {
    configFile.close();
    return "Config file size is too large";
  }
  String temp = configFile.readString();
  configFile.close();
  return temp;
}

// ------------- Запись строки в файл
String writeFile(String fileName, String strings ) {
  File configFile = SPIFFS.open("/" + fileName, "w");
  if (!configFile) {
    return "Failed to open config file";
  }
  configFile.print(strings);
  //strings.printTo(configFile);
  configFile.close();
  return "Write sucsses";
}

// ------------- Чтение значения json
String jsonRead(String json, String name) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name].as<String>();
}

// ------------- Чтение значения json
int jsonReadtoInt(String json, String name) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name];
}

// ------------- Запись значения json String
String jsonWrite(String json, String name, String volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name] = volume;
  json = "";
  root.printTo(json);
  return json;
}


// ------------- Запись значения json int
String jsonWrite(String json, String name, int volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name] = volume;
  json = "";
  root.printTo(json);
  return json;
}

// --------------Создание данных для графика
String graf(int datas, int points, int refresh) {
  String root = "{}";  // Формировать строку для отправки в браузер json формат
  // {"data":[1],"points":"10","refresh":"1","title":"Analog"}
  // Резервируем память для json обекта буфер может рости по мере необходимти, предпочтительно для ESP8266
  DynamicJsonBuffer jsonBuffer;
  // вызовите парсер JSON через экземпляр jsonBuffer
  JsonObject& json = jsonBuffer.parseObject(root);
  // Заполняем поля json
  JsonArray& data = json.createNestedArray("data");
  data.add(datas);
  json["points"] = points;
  json["refresh"] = refresh;
  // Помещаем созданный json в переменную root
  root = "";
  json.printTo(root);
  return root;
}

// --------------Добавить данные в масивв json



//------------------Запуск конфигурации в соответствии с разделом строки
String modulesInit(String json, String nameArray) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& All = jsonBuffer.parseObject(json);
  JsonArray& nestedArray = All[nameArray].asArray();
  int k = nestedArray.size();
  for (int i = 0; i <= k - 1; i++) {
    String temp =  All[nameArray][i]["type"];
    Serial.println( temp );
    sCmd.readStr(temp);
  }
  return "OK";
}



// ------------- Данные статистики
void statistics() {
  String urls = "http://backup.privet.lv/visitors.php?";
  urls += WiFi.macAddress().c_str();
  urls += "&";
  urls += jsonRead(configJson, "configs");
  getURL(urls);
}

// Запрос на удаленный URL
String getURL(String urls) {
  String answer = "";
  HTTPClient http;
  http.begin(urls); //HTTP
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    answer = http.getString();
  }
  http.end();
  return answer;
}

// -------------- Регистрация модуля
//{"ip":"192.168.0.103","SSDP":"Sonoff","module":["sonoff"]}
void modulesReg(String modName) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(modules);
  json["SSDP"] = jsonRead(configJson, "SSDP");
  json["space"] = jsonRead(configJson, "space");
  JsonArray& data = json["module"].asArray();
  data.add(modName);
  modules = "";
  json.printTo(modules);
}

// --------------------Выделяем строку до маркера
String selectToMarker (String str, String found) {
  int p = str.indexOf(found);
  return str.substring(0, p);
}

// -------------------Выделяем строку от конца строки до маркера
String selectToMarkerLast (String str, String found) {
  int p = str.lastIndexOf(found);
  return str.substring(p + 1);
}

//----------------------Удаляем все до символа разделителя
String deleteBeforeDelimiter(String str, String found) {
  int p = str.indexOf(found) + found.length();
  return str.substring(p);
}

// ------------------- Проверка доступности pin
int pinOk(int pin) {
  if ((pin > 5 && pin < 12) || pin > 16) return 32;
  return pin;
}
