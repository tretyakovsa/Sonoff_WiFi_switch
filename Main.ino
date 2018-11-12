// --------------------- Загрузка переменных -------------------------------------------------------
void setupToInit() {
  sendStatus(timeS, "00:00:00");
  setupToOptions(langS);
  setupToOptions(ssdpS);
  setupToOptions(spaceS);
  setupToOptions(timeZoneS);
  setupToOptions(ddnsNameS);
  setupToOptions(ddnsPortS);
  setupToOptions(spiffsDataS);
  setupToOptions(buildDataS);
  setupToOptions(langS);
  sendOptions("flashChip", String(ESP.getFlashChipId(), HEX));
  sendOptions("ideFlashSize", ESP.getFlashChipSize());
  sendOptions("realFlashSize", ESP.getFlashChipRealSize());
  sendOptions("flashChipSpeed", ESP.getFlashChipSpeed() / 1000000);
  sendOptions("cpuFreqMHz", ESP.getCpuFreqMHz());
  FlashMode_t ideMode = ESP.getFlashChipMode();
  sendOptions("FreeSketchSpace", ESP.getFreeSketchSpace());
  sendOptions("flashChipMode", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
  String configs = getSetup(configsS);
  configs.toLowerCase();
  String test = readFile("configs/" + configs + ".txt", 4096);
  test.replace("\r\n", "\n");
  test += "\n";
  goCommands(test);
  test = emptyS;
  sendOptions(macS, WiFi.macAddress().c_str());
  sendOptions(ipS, WiFi.localIP().toString());
  sendOptions(macS, WiFi.macAddress().c_str());
  sendOptions("voice", emptyS);
  jsonWrite(modules, ipS, getOptions(ipS));
  jsonWrite(modules, spaceS, getSetup(spaceS));
  jsonWrite(modules, langS, getSetup(langS));
  jsonWrite(modules, ssdpS, getSetup(ssdpS));
  initPuls();

}

// --------------------Выделяем строку до маркера --------------------------------------------------
String selectToMarker (String str, String found) {
  int p = str.indexOf(found);
  return str.substring(0, p);
}

// -------------------Выделяем строку от конца строки до маркера ---------------------
String selectToMarkerLast (String str, String found) {
  int p = str.lastIndexOf(found);
  return str.substring(p + found.length());
}

//----------------------Удаляем все до символа разделителя -----------------
String deleteBeforeDelimiter(String str, String found) {
  int p = str.indexOf(found) + found.length();
  return str.substring(p);
}
//----------------------Удаляем все до символа разделителя -----------------
String deleteBeforeDelimiterTo(String str, String found) {
  int p = str.indexOf(found);
  return str.substring(p);
}
// -------------------Удаляем строку от конца строки до маркера ---------------------
String deleteToMarkerLast (String str, String found) {
  int p = str.lastIndexOf(found);
  return str.substring(0, p);
}

// ------------- Данные статистики -----------------------------------------------------------
void statistics() {
  String urls = "http://backup.privet.lv/visitors/?";
  urls += WiFi.macAddress().c_str();
  urls += "&";
  urls += getSetup(configsS);
  urls += "&";
  urls += ESP.getResetReason();
  urls += "&";
  urls += getSetup(spiffsDataS);
  String stat = getURL(urls);
  sendOptions(messageS, jsonRead(stat, messageS));
}


// ------------- Запрос на удаленный URL -----------------------------------------
String getURL(String urls) {
  String answer;
  HTTPClient http;
  http.begin(urls); //HTTP
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    answer = http.getString();
  }
  http.end();
  return answer;
}
//------------------Выполнить все команды по порядку из строки разделитель \r\n  \n
String goCommands(String inits) {
  //Serial.println(inits);
  String temp;
  String rn = "\n";
  inits += rn;
  //  Serial.println(writeFile("inits.txt", inits));
  do {
    temp = selectToMarker (inits, rn);

    //    Serial.println(temp);
    sCmd.readStr(temp);
    inits = deleteBeforeDelimiter(inits, rn);
  } while (inits.indexOf(rn) != 0);
  return "OK";
}
// ------------- Чтение файла в строку --------------------------------------
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
// ------------- Запись строки в файл --------------------------
String writeFile(String fileName, String strings ) {
  File configFile = SPIFFS.open("/" + fileName, "w");
  if (!configFile) {
    return "Failed to open file";
  }
  configFile.print(strings);
  configFile.close();
  return "Write sucsses";
}
// ------------- Запись файла конфигурации ----------------------------------
void saveConfigSetup () {
  writeFile(fileConfigS, configSetup );
}

// ------------- Проверка занятости пина --------------------------
/*
   Алгоритм
   Провнряем свободен ли пин если нет вернем 17
   Если свободен то займем pins[pin] = true;

*/
uint8_t pinTest(uint8_t pin) {
  //Serial.print(pin);
  //Serial.print("=");
  if (pin > 20) {
    pin = 17;
  } else {
    if (pins[pin]) {
      pin = 17 ;
    }
    else {
      pins[pin] = true;
      if (getOptions("flashChipMode") != "DOUT") {
        if (pin > 5 && pin < 12) pin = 17 ;
        if (pin == 1 || pin == 3)  Serial.end();
      } else {
        if ( (pin > 5 && pin < 9) ||  pin == 11) pin = 17 ;
      }
    }
  }
  //Serial.println(pin);
  return pin;
}
uint8_t pinTest(uint8_t pin, boolean multi) {
  if (pin > 20) {
    pin = 17;
  } else {
    pins[pin] = !multi;
    if (pins[pin]) {
      pin = 17 ;
    }
    else {
      pins[pin] = true;
      if (getOptions("flashChipMode") != "DOUT") {
        if (pin > 5 && pin < 12) pin = 17 ;
        if (pin == 1 || pin == 3)  Serial.end();
      } else {
        if ( (pin > 5 && pin < 9) ||  pin == 11) pin = 17 ;
      }
    }
  }
  return pin;
}

void testPin() {
  for (int i = 0; i <= 20; i++) {
    Serial.print("pins");
    Serial.print(i);
    Serial.print("=");
    Serial.println(pins[i]);
  }
}

// -------------- Регистрация модуля
void modulesReg(String modName) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(modules);
  json[ssdpS] = jsonRead(configJson, ssdpS);
  json[spaceS] = jsonRead(configJson, spaceS);
  JsonArray& data = json["module"].asArray();
  data.add(modName);
  modules = emptyS;
  json.printTo(modules);
}
// -------------- Регистрация команд
void commandsReg(String comName) {
  if (regCommands.indexOf(comName) == -1) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(regCommands);
    JsonArray& data = json["command"].asArray();
    data.add(comName);
    regCommands = emptyS;
    json.printTo(regCommands);
  }
}
// Запись данных в файл с частотой 1 секунда и более. Максимальное количество данных в суточном файле 1440 значений
void safeDataToFile(int inter, String par, uint16_t data) {
  yield();
  // Формируем зоголовок (префикс) Интервал, Параметр, размер_параметра
  uint16_t dataSize = sizeof(data);
  String prifexFile;
  prifexFile += inter;
  prifexFile += "," + par;
  prifexFile += ",";
  prifexFile += dataSize;
  prifexFile += ":";
  uint16_t prifexLen = prifexFile.length(); //Размер префикса

  // Сделаем имя файла
  String fileName = GetDate();
  fileName = deleteBeforeDelimiter(fileName, " "); // удалим день недели
  fileName.replace(" ", ".");
  fileName.replace("..", "."); // Заменяем пробелы точками
  fileName = par + "/" + fileName + ".txt"; // Имя файла параметр в виде директории и дата
  fileName.toLowerCase(); //fileName = "san aug 31 2018"; Имя файла строчными буквами
  File configFile = SPIFFS.open("/" + fileName, "a"); // Открываем файл на добавление
  size_t size = configFile.size();
  yield();
  if (size == 0) {
    configFile.print(prifexFile);
  }
  size = configFile.size();
  // Получим время и определим позицию в файле
  String time = GetTime();
  //time = "00:15:00";
  int timeM = timeToMin(time); // Здесь количество минут с начала суток
  timeM = timeM / inter;
  int poz = timeM * dataSize + prifexLen + 1; // позиция в которую нужно записать.
  int endF = (size - prifexLen) * dataSize + prifexLen + 1; // позиция конца файла
  if (poz >= endF) { // если файл имел пропуски в записи данных
    int i = (poz - endF) / dataSize;
    for (int j = 0; j < i; j++) { // Заполним недостающие данные
      for (int d = 0; d < dataSize; d++) {
        yield();
        configFile.write(0);    // нулями
      }
    }
  }
  yield();
  configFile.write(data >> 8); // добавим текущие
  configFile.write(data);      // данные
  configFile.close();
}

