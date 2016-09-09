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
  _ssid = ssid;
  String password = json["ssidPassword"];
  _password = password;
  int setAP = json["onOffAP"];
  _setAP=setAP;
  String _times1 = json["times1"];
  times1 = _times1;
  String _times2 = json["times2"];
  times2 = _times2;
  return true;
}

bool saveConfig() {
  StaticJsonBuffer<300> jsonBuffer;
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
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    //Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  return true;
}

void CheckFlashConfig() {
  uint32_t realSize = ESP.getFlashChipRealSize();
  uint32_t ideSize = ESP.getFlashChipSize();
  FlashMode_t ideMode = ESP.getFlashChipMode();

  //Serial.printf("Flash real id:   %08X\n", ESP.getFlashChipId());
  //Serial.printf("Flash real size: %u\n\n", realSize);

  //Serial.printf("Flash ide  size: %u\n", ideSize);
  //Serial.printf("Flash ide speed: %u\n", ESP.getFlashChipSpeed());
  //Serial.printf("Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));

  if (ideSize != realSize) {
    //Serial.println("Flash Chip configuration wrong!\n");
  } else {
    //Serial.println("Flash Chip configuration ok.\n");
  }
  delay(5000);
}
