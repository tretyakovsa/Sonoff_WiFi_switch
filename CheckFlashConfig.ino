bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    //Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    //Serial.println("Config file size is too large");
    return false;
  }

  // Выделим буфер для хранения содержимого файла.
  std::unique_ptr<char[]> buf(new char[size]);

  // Мы не используем строку здесь, потому что библиотека Arduino Json требует ввода
  // буфер, чтобы быть изменчивым. Если вы не используете ArduinoJson, вы можете также
  // использование configFile.readString вместо этого.
  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    //Serial.println("Failed to parse config file");
    return false;
  }
  String ssidAP = json["ssidAPName"];
  _ssidAP = ssidAP;
  String passwordAP = json["ssidAPPassword"];
  _passwordAP = passwordAP;

  String _timezone = json["timezone"];
  timezone = _timezone.toInt();

  String SSDPName = json["SSDPName"];
  SSDP_Name = SSDPName;
  String ssid = json["ssidName"];
   Serial.println(ssid);
  _ssid = ssid;
   Serial.println(_ssid);
  String password = json["ssidPassword"];
  Serial.println(password);
  _password = password;
  Serial.println(_password);
  int setAP = json["onOffAP"];
  _setAP=setAP;
  String _times1 = json["times1"];
  times1 = _times1;
  String _times2 = json["times2"];
  times2 = _times2;
    String _DDNS = json["DDNS"];
  DDNS = _DDNS;
  return true;
}

bool saveConfig() {
  StaticJsonBuffer<400> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["onOffAP"] = _setAP;
  json["SSDPName"] = SSDP_Name;
  json["ssidAPName"] = _ssidAP;
  json["ssidAPPassword"] = _passwordAP;
  json["ssidName"] = _ssid;
  json["ssidPassword"] = _password;
  json["timezone"] = timezone;
  json["times1"] = times1;
  json["times2"] = times2;
  json["DDNS"] = DDNS;
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    //Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  return true;
}

