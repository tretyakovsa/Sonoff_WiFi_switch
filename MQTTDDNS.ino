/*
    Для использования вкладки требуется добавить в заголовке скетча следуюший код
  #include <PubSubClient.h>
  WiFiClient wclient;
  PubSubClient client(wclient);
*/

void callback(const MQTT::Publish& pub)
{
  Serial.print(pub.topic()); // выводим в сериал порт название топика
  Serial.print(" => ");
  Serial.print(pub.payload_string()); // выводим в сериал порт значение полученных данных
  Serial.println();
  String payload = pub.payload_string();
  Serial.println(payload);

  if ( pub.payload_string() == "HELLO" ) {  // handshaking

    loadnWidgets();
  }

  if (String(pub.topic()) == prefix + "/" + chipID + "/RELE_1_not/control") // проверяем из нужного ли нам топика пришли данные
    //if (String(pub.topic()) == prefix + "/"+chipID + "/RELE_1_not/status") // проверяем из нужного ли нам топика пришли данные
  {
    int stled = payload.toInt(); // преобразуем полученные данные в тип integer
    //Serial.println(stled);
    if (stled == 1)   command = "relayon";
    if (stled == 0)   command = "relayoff";
    //Serial.println(command);

  }

}

void initMQTT() {
 // --------------Установить данные MQTT
  HTTP.on("/mqtt", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasArg("server")) {
      configSetup = jsonWrite(configSetup, "mqttServer", request->arg("server"));
    }
    if (request->hasArg("port")) {
      configSetup = jsonWrite(configSetup, "mqttPort", request->arg("port").toInt());
    }
    if (request->hasArg("user")) {
      configSetup = jsonWrite(configSetup, "mqttUser", request->arg("user"));
    }
    if (request->hasArg("pass")) {
      configSetup = jsonWrite(configSetup, "mqttPass", request->arg("pass"));
    }
      saveConfigSetup ();
  client.disconnect();
  MQTT_Pablush();
   request->send(200, "text/plain", "Ok");

  });
  modulesReg("mqtt");
  MQTT_Pablush();
}

void MQTT_Pablush() {
  String mqtt_server = jsonRead(configSetup, "mqttServer");
  if ((mqtt_server != "")) {
    client.set_server(mqtt_server, jsonReadtoInt(configSetup, "mqttPort"));
    // подключаемся к MQTT серверу
    if (WiFi.status() == WL_CONNECTED) {
      if (!client.connected()) {
        //Serial.println("Connecting to MQTT server");
        if (client.connect(MQTT::Connect(chipID)
                           .set_auth(jsonRead(configSetup, "mqttUser"), jsonRead(configSetup, "mqttPass")))) {
          //Serial.println("Connected to MQTT server");
          client.set_callback(callback);
          client.subscribe(prefix);  // Для приема получения HELLOW и подтверждения связи
          client.subscribe(prefix + "/+/+/control"); // Подписываемся на топики control
          //client.subscribe("/" + chipID + "/RELE_1"); // подписываемся по топик с данными для светодиода
          loadnWidgets();
        } else {
          //Serial.println("Could not connect to MQTT server");
        }
      }
    }
  }
}

void  handleMQTT() {
  if (client.connected()) client.loop();
  else {
    MQTT_Pablush();
    //Serial.println("MQTT");
    }
}

// Читаем и отправляем виджеты на сервер
bool loadnWidgets() {

  DynamicJsonBuffer jsonBuffer;
  JsonObject& Widgets = jsonBuffer.parseObject(readFile("config.widgets.json", 4096));
  JsonArray& WidgetsArray = Widgets["nWidgets"].asArray();
  int j = WidgetsArray.size();
  if (j != 0) {
    for (int i = 0; i <= j - 1; i++) {
      Widgets["nWidgets"][i]["topic"] = prefix + "/" + chipID + Widgets["nWidgets"][i]["topic"].as<String>();
      Widgets["nWidgets"][i]["descr"] = jsonRead(configSetup, "SSDP");
      String thing_config = Widgets["nWidgets"][i].as<String>();
      //Serial.println(thing_config);
      client.publish(MQTT::Publish(prefix + "/" + chipID + "/config", thing_config).set_qos(1));
    }
  }
  return true;
}




