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
  String Config = configFile.readString();
  Serial.println(Config);
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(Config);
  spiffsData = root["spiffsData"].as<String>();
  buildData = root["buildData"].as<String>();
  ssidApName = root["ssidApName"].as<String>();
  ssidApPass = root["ssidApPass"].as<String>();
  timezone = root["timeZone"];
  ssdpName = root["ssdpName"].as<String>();
  ssidName = WiFi.SSID ();
  //root["ssidName"].as<String>();
  spaceName = root["space"].as<String>();
  ssidPass = "**********";
  //root["ssidPass"].as<String>();
  subnet = root["subnet"].as<String>();
  getway = root["getway"].as<String>();
  dns = root["dns"].as<String>();
  ip = root["ip"].as<String>();
  checkboxIP = root["checkboxIP"];
  Language = root["lang"].as<String>();
  ddns = root["ddns"].as<String>();
  ddnsName = root["ddnsName"].as<String>();
  ddnsPort = root["ddnsPort"];
  pirTime = root["pirTime"];
  mqtt_server = root["mqtt_server"].as<String>();
  mqtt_port = root["mqtt_port"];
  mqtt_user = root["mqtt_user"].as<String>();
  mqtt_pass = root["mqtt_pass"].as<String>();
  configs = root["configs"].as<String>();
  return true;
}

bool saveConfig() {
  String Config = "{}";
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(Config);
  json["spiffsData"] = spiffsData;
  json["buildData"] = buildData;
  json["ssdpName"] = ssdpName;
  json["space"] = spaceName;
  json["ssidApName"] = ssidApName;
  json["ssidApPass"] = ssidApPass;
  //json["ssidName"] = ssidName;
  //json["ssidPass"] = ssidPass;
  json["timeZone"] = timezone;
  json["subnet"] = subnet;
  json["getway"] = getway;
  json["dns"] = dns;
  json["ip"] = ip;
  json["checkboxIP"] = checkboxIP;
  json["configs"] = configs;
  json["lang"] = Language;
  json["ddns"] = ddns;
  json["ddnsName"] = ddnsName;
  json["ddnsPort"] = ddnsPort;
  json["pirTime"] = pirTime;

  json["mqtt_server"] = mqtt_server;
  json["mqtt_port"] = mqtt_port;
  json["mqtt_user"] = mqtt_user;
  json["mqtt_pass"] = mqtt_pass;
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
  json["space"] = spaceName;
  JsonArray& data = json["module"].asArray();
  data.add(modName);
  modulesNew = "";
  json.printTo(modulesNew);
}


