void start_init() {
  SPIFFS.begin();
  chipID = String( ESP.getChipId() ) + "-" + String( ESP.getFlashChipId() );
  initCMD();
  // ----------------- начинаем загрузку
  configSetup = readFile(fileConfigS, 4096);                      //Получим файл конфигурации configSetup можно считать по запросу
  sendStatus(timeS, "00:00:00");                                         //Добавить раздел времени в configJson
  sendOptions(langS, jsonRead(configSetup, langS));                      //Скопировать ключ "lang" из configSetup в configOptions
  sendOptions(ssdpS, jsonRead(configSetup, ssdpS));                      //Скопировать ключ "ssdp"  из configSetup в configOptions
  sendOptions(timeZoneS, jsonRead(configSetup, timeZoneS));              //Скопировать ключ "timeZone"  из configSetup в configOptions
  sendOptions(spaceS, jsonRead(configSetup, spaceS));                    //Скопировать ключ "space"  из configSetup в configOptions
  sendOptions(ddnsNameS, jsonRead(configSetup, ddnsNameS));              //Скопировать ключ "ddnsName"  из configSetup в configOptions
  sendOptions(ddnsPortS, jsonRead(configSetup, ddnsPortS));              //Скопировать ключ "ddnsPort"  из configSetup в configOptions
  sendOptions(spiffsDataS, jsonRead(configSetup, spiffsDataS));          //Скопировать ключ "spiffsData"  из configSetup в configOptions
  sendOptions(buildDataS, jsonRead(configSetup, buildDataS));            //Скопировать ключ "buildData"  из configSetup в configOptions
  jsonWrite(modules, langS, jsonRead(configSetup, langS));
  String configs = jsonRead(configSetup, configsS);
  configs.toLowerCase();
  sendOptions("flashChip", String(ESP.getFlashChipId(), HEX));
  sendOptions("ideFlashSize", ESP.getFlashChipSize());
  sendOptions("realFlashSize", ESP.getFlashChipRealSize());
  sendOptions("flashChipSpeed", ESP.getFlashChipSpeed()/1000000);
  sendOptions("cpuFreqMHz", ESP.getCpuFreqMHz());
  FlashMode_t ideMode = ESP.getFlashChipMode();
  sendOptions("flashChipMode",(ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
  initWIFI();
  initHTTP();
  initUpgrade();
  initFS();
  initSSDP();
  initScenary();
  String test = readFile("configs/"+configs+".txt", 4096);
  test.replace("\r\n", "\n");
  test +="\n";
  goCommands(test);
  test = "";
  jsonWrite(configSetup, macS, WiFi.macAddress().c_str());
  jsonWrite(configSetup, ipS, WiFi.localIP().toString());
  sendOptions(macS, WiFi.macAddress().c_str());
   testJson = configJson;
  Serial.println("");
}

// ------------- Чтение файла в строку
String readFile(String fileName, size_t len ) {
  File configFile = SPIFFS.open("/" + fileName, "r");
  if (!configFile) {
    return "Failed";
  }
  size_t size = configFile.size();
  if (size > len) {
    configFile.close();
    return "Large";
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
  configFile.close();
  return "Write sucsses";
}


// ------------- Чтение значения json
String jsonRead(String &json, String name) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name].as<String>();
}
// ------------- Чтение значения json
int jsonReadtoInt(String &json, String name) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name];
}
// ------------- Чтение значения json
float jsonReadtoFloat(String &json, String name) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name].as<float>();
}

// ------------- Запись значения json String
String jsonWrite(String &json, String name, String volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name] = volume;
  json = "";
  root.printTo(json);
  return json;
}

// ------------- Запись значения json int
String jsonWrite(String &json, String name, int volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name] = volume;
  json = "";
  root.printTo(json);
  return json;
}

// ----------------- Процедуры статуса
// Меняем любой статус в /config.live.json configJson
// Если нужновызвать проверку скрипта то присвоить ответ переенной flag
boolean sendStatus(String Name, String volume) {
  jsonWrite(configJson, Name, volume);
  return true;
}
boolean sendStatus(String Name, int volume) {
  jsonWrite(configJson, Name, volume);
  return true;
}

// Читаем любой стстус /config.live.json configJson
// Вернет String
String getStatus(String Name) {
  return jsonRead(configJson, Name);
}
// Вернет int
int getStatusInt(String Name) {
  return jsonReadtoInt(configJson, Name);
}
// Вернет float
float getStatusFloat(String Name) {
  return jsonReadtoFloat(configJson, Name);
}


// ----------------- Процедуры Параметров
// Меняем любой статус в /config.live.json configJson
// Если нужновызвать проверку скрипта то присвоить ответ переенной flag
boolean sendOptions(String Name, String volume) {
  jsonWrite(configOptions, Name, volume);
  return true;
}
boolean sendOptions(String Name, int volume) {
  jsonWrite(configOptions, Name, volume);
  return true;
}

// Вернет из json строки в configOptionsint значение ключа Name как String
String getOptions(String Name) {
  return jsonRead(configOptions, Name);
}
// Вернет из json строки в configOptionsint значение ключа Name как int
int getOptionsInt(String Name) {
  return jsonReadtoInt(configOptions, Name);
}
// Вернет из json строки в configOptionsint значение ключа Name как float
float getOptionsFloat(String Name) {
  return jsonReadtoFloat(configOptions, Name);
}

void saveConfigSetup () {
  writeFile("config.save.json", configSetup );
}

void httpOkText() {
  HTTP.send(200, "text/plain", "Ok");
}
void httpOkText(String text) {
  HTTP.send(200, "text/plain", text);
}
void httpOkJson(String text) {
  HTTP.send(200, "application/json", text);
}

uint8_t pinTest(uint8_t pin) {
  if (pins[pin]) pin = 17 ;
  pins[pin] = true;
  if (((pin > 5 && pin < 12) || pin > 16)) pin = 17 ;
  if (pin == 1 || pin == 3)  Serial.end();
   return pin;
}

uint8_t pinTest(uint8_t pin, boolean multi){
 pins[pin] = !multi;
 if (pins[pin]) pin = 17 ;
  pins[pin] = true;
  if (((pin > 5 && pin < 12) || pin > 16)) pin = 17 ;
  if (pin == 1 || pin == 3)  Serial.end();
   return pin;
}


// ------------- Данные статистики
void statistics() {
  String urls = "http://backup.privet.lv/visitors/?";
  urls += WiFi.macAddress().c_str();
  urls += "&";
  urls += jsonRead(configSetup, configsS);
  urls += "&";
  urls += ESP.getResetReason();
  urls += "&";
  urls += jsonRead(configSetup, spiffsDataS);
  String stat = getURL(urls);
  sendOptions("message", jsonRead(stat, "message"));
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


// -------------- Регистрация модуля
void modulesReg(String modName) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(modules);
  json[ssdpS] = jsonRead(configJson, ssdpS);
  json[spaceS] = jsonRead(configJson, spaceS);
  JsonArray& data = json["module"].asArray();
  data.add(modName);
  modules = "";
  json.printTo(modules);
}

// Читает аргументы из команд каждый слежующий вызов читает следующий аргумент возвращает String
String readArgsString() {
  String arg;
  arg = sCmd.next();    // Get the next argument from the SerialCommand object buffer
  if (arg == "") arg = "";
  return arg;
}
// Читает аргументы из команд каждый слежующий вызов читает следующий аргумент возвращает Int
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

//------------------Выполнить все команды по порядку из строки разделитель \r\n
String goCommands(String inits) {
  String temp = "";
  String rn = "\n";
  inits += rn;
  do {
    temp = selectToMarker (inits, rn);
    //Serial.println(temp);
    sCmd.readStr(temp);
    inits = deleteBeforeDelimiter(inits, rn);
  } while (inits.indexOf(rn) != 0);
  return "OK";
}

// -------------- Регистрация команд
void commandsReg(String comName) {
  if (regCommands.indexOf(comName)==-1){
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(regCommands);
  JsonArray& data = json["command"].asArray();

  data.add(comName);
  regCommands = "";
  json.printTo(regCommands);
  }
}

String addressToString(DeviceAddress deviceAddress)
{
  String addr ="";
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) addr += "0";
    addr += String(deviceAddress[i],HEX);
  }
  return addr;
}
