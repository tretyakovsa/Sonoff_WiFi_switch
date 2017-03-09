bool loadConfig() {
  File configFile = SPIFFS.open("/config.save.json", "r");
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
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(jsonConfig);
  ssidApName = root["ssidApName"].as<String>();
  ssidApPass = root["ssidApPass"].as<String>();
  timezone = root["timeZone"];
  ssdpName = root["ssdpName"].as<String>();
  ssidName = root["ssidName"].as<String>();
  ssidPass = root["ssidPass"].as<String>();
  Language = root["lang"].as<String>();
  ddns = root["ddns"].as<String>();
  ddnsName = root["ddnsName"].as<String>();
  ddnsPort = root["ddnsPort"];
  pirTime = root["pirTime"];
  return true;
}

bool saveConfig() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(jsonConfig);
  json["ssdpName"] = ssdpName;
  json["ssidApName"] = ssidApName;
  json["ssidApPass"] = ssidApPass;
  json["ssidName"] = ssidName;
  json["ssidPass"] = ssidPass;
  json["timeZone"] = timezone;

  json["lang"] = Language;
  json["ddns"] = ddns;
  json["ddnsName"] = ddnsName;
  json["ddnsPort"] = ddnsPort;
  json["pirTime"] = pirTime;
  File configFile = SPIFFS.open("/config.save.json", "w");
  if (!configFile) {
    //Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  return true;
}
//{"ip":"192.168.0.103","SSDP":"Sonoff","module":["sonoff"]}
void modulesReg(String modName) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(modulesNew);
  json["ip"] = WiFi.localIP().toString();
  json["SSDP"] = ssdpName;
  JsonArray& data = json["module"].asArray();
  data.add(modName);
  modulesNew="";
  json.printTo(modulesNew);
}
