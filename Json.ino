// ------------- Чтение значения json -------------------
String jsonRead(String &json, String name) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name].as<String>();
}
int jsonReadToInt(String &json, String name) {
  return jsonRead(json, name).toInt();
}
float jsonReadToFloat(String &json, String name) {
  return jsonRead(json, name).toFloat();
}
// -------------- Чтение configSetup -------------------------------
String getSetup(String Name) {
  return jsonRead(configSetup, Name);
}
int getSetupInt(String Name) {
  return jsonReadToInt(configSetup, Name);
}
float getSetupFloat(String Name) {
  return jsonReadToFloat(configSetup, Name);
}
// -------------- Чтение configOptions --------------------
String getOptions(String Name) {
  return jsonRead(configOptions, Name);
}
int getOptionsInt(String Name) {
  return jsonReadToInt(configOptions, Name);
}
float getOptionsFloat(String Name) {
  return jsonReadToFloat(configOptions, Name);
}
// -------------- Чтение configJson     -----------------------------------------------
String getStatus(String Name) {
  return jsonRead(configJson, Name);
}
int getStatusInt(String Name) {
  return jsonReadToInt(configJson, Name);
}
float getStatusFloat(String Name) {
  return jsonReadToFloat(configJson, Name);
}

// ------------- Запись значения json String ----------------
void jsonWrite(String &json, String name, String volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name] = volume;
  json = emptyS;
  root.printTo(json);
}
// ------------- Запись значения json int ----------------
void jsonWrite(String &json, String name, int volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name] = volume;
  json = emptyS;
  root.printTo(json);
}
// ------------- Запись значения json float ----------------
void jsonWrite(String &json, String name, float volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name] = volume;
  json = emptyS;
  root.printTo(json);
}
// ------------- Отправить данные в Setup String ------------------------------
void sendSetup(String Name, String volume) {
  jsonWrite(configSetup, Name, volume);
}
// ------------- Отправить данные в Setup int -----------------------
void sendSetup(String Name, int volume) {
  jsonWrite(configSetup, Name, volume);
}
// ------------- Отправить данные в Setup float ------------
void sendSetupF(String Name, float volume) {
  jsonWrite(configSetup, Name, volume);
}
// ------------- Отправить данные в Status String ------------------------------
boolean sendStatus(String Name, String volume) {
  jsonWrite(configJson, Name, volume);
  return true;
}
// ------------- Отправить данные в Status int -----------------------
boolean sendStatus(String Name, int volume) {
  jsonWrite(configJson, Name, volume);
  return true;
}
// ------------- Отправить данные в Status float ----------
boolean sendStatusF(String Name, float volume) {
  jsonWrite(configJson, Name, volume);
  return true;
}
// ------------- Отправить данные в Options String ------------------------------
void sendOptions(String Name, String volume) {
  jsonWrite(configOptions, Name, volume);
}
// ------------- Отправить данные в Options int -----------------------
void sendOptions(String Name, int volume) {
  jsonWrite(configOptions, Name, volume);
}
// ------------- Отправить данные в Options float ----------
void sendOptionsF(String Name, float volume) {
  jsonWrite(configOptions, Name, volume);
}
// ------------- Копируем параметр из Setup Options
void setupToOptions(String Name){
  sendOptions(Name,getSetup(Name));
  }
// ------------- Копируем параметр из Setup Options
void setupToStatus(String Name){
  sendStatus(Name,getSetup(Name));
  }


