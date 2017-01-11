bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

// загружаем файл конфигурации в глобальную переменную
  jsonConfig = configFile.readString();
  Serial.println(jsonConfig);
  //
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(jsonConfig);
    _ssidAP = root["ssidAPName"].as<String>();
    _passwordAP = root["ssidAPPassword"].as<String>();
    timezone = root["timezone"];
    SSDP_Name = root["SSDPName"].as<String>();
    _ssid = root["ssidName"].as<String>();
    _password = root["ssidPassword"].as<String>();
    times1 = root["times1"].as<String>();
    times2 = root["times2"].as<String>();
    Language = root["Lang"].as<String>();
    DDNS = root["DDNS"].as<String>();

  return true;
}

bool saveConfig() {
   DynamicJsonBuffer jsonBuffer;
   JsonObject& json = jsonBuffer.parseObject(jsonConfig);
   json["SSDPName"] = SSDP_Name;
  json["ssidAPName"] = _ssidAP;
  json["ssidAPPassword"] = _passwordAP;
  json["ssidName"] = _ssid;
  json["ssidPassword"] = _password;
  json["timezone"] = timezone;
  json["times1"] = times1;
  json["times2"] = times2;
  json["Lang"] = Language;
  json["DDNS"] = DDNS;
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    //Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  return true;
  }


