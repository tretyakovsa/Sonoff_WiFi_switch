/*
    Для использования вкладки требуется добавить в заголовке скетча следуюший код
  #include <PubSubClient.h>
  WiFiClient wclient;
  PubSubClient client(wclient);
*/
void initMQTT() {
  HTTP.on("/mqtt", HTTP_GET, []() {
  jsonWrite(configSetup, mqttServerS,  HTTP.arg("server"));
  jsonWrite(configSetup, mqttPortS,  HTTP.arg("port").toInt());
  jsonWrite(configSetup, mqttUserS,  HTTP.arg("user"));
  jsonWrite(configSetup, mqttPassS,  HTTP.arg("pass"));
  saveConfigSetup ();
  client.disconnect();
  MQTT_Pablush();
    httpOkText();
  });
  modulesReg("mqtt");
  MQTT_Pablush();
}

void MQTT_Pablush() {
  String mqtt_server = jsonRead(configSetup, mqttServerS);
  if ((mqtt_server != "")) {
    Serial.println("MQTT");
    client.set_server(mqtt_server, jsonReadtoInt(configSetup, mqttPortS));
    // подключаемся к MQTT серверу
    if (WiFi.status() == WL_CONNECTED) {
      if (!client.connected()) {
        if (client.connect(MQTT::Connect(chipID)
                           .set_auth(jsonRead(configSetup, mqttUserS), jsonRead(configSetup, mqttPassS)))) {
          client.set_callback(callback);
          client.subscribe(prefix);  // Для приема получения HELLOW и подтверждения связи
          client.subscribe(prefix + "/+/+/control"); // Подписываемся на топики control
          //client.subscribe( prefix + "/" + chipID + "/+/control"); // Подписываемся на топики control
        } else {
        }
      }
    }
  }
}

void callback(const MQTT::Publish& pub)
{
  Serial.print(pub.topic()); // выводим в сериал порт название топика
  Serial.print(" => ");
  Serial.print(pub.payload_string()); // выводим в сериал порт значение полученных данных
  Serial.println();
  String payload = pub.payload_string();
  Serial.println(payload);
  if ( pub.payload_string() == "HELLO" ) {
    loadnWidgets();
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
  String prex = prefix + "/" + chipID;
  if (j != 0) {
    for (int i = 0; i <= j - 1; i++) {
      String thing_config = Widgets["nWidgets"][i].as<String>();
      thing_config = jsonWrite(thing_config, "topic",  prex+ jsonRead(thing_config, "topic"));
      thing_config = jsonWrite(thing_config, "page", jsonRead(configSetup, spaceS));
      Serial.println(thing_config);
      client.publish(MQTT::Publish(prex + "/config", thing_config).set_qos(1));
    }
  }
  return true;
}

// --------------------- Включаем DDNS
void initDDNS() {
  HTTPWAN = ESP8266WebServer (jsonReadtoInt(configSetup, ddnsPortS));
  HTTP.on("/ddns", handle_ddns);               // Установка параметров ddns
  // ------------------Выполнение команды из запроса
  HTTPWAN.on("/cmd", HTTP_GET, []() {
    String com = HTTPWAN.arg("command");
    sendStatus("test", com);
    sCmd.readStr(com);
    HTTPWAN.send(200, "text/plain", com);
  });
  HTTPWAN.on("/", HTTP_GET, []() {
    String str = jsonRead(configSetup, ddnsNameS);
    HTTPWAN.send(200, "text/plain", str);
  });
   HTTPWAN.onNotFound([]() {
   Serial.println("TEST DDNS");
      HTTPWAN.send(404, "text/plain", "FileNotFound");
  });

    // задача синхронизайия с сервером ddns каждые 6 минут
 ts.add(8, 600000, [&](void*) {
    ip_wan();
  }, nullptr, true);
  ip_wan();
  HTTPWAN.begin();
  modulesReg(ddnsS);
}

// ------------------------------Установка параметров ddns
void handle_ddns() {
jsonWrite(configSetup, ddnsS,  HTTP.arg(ddnsS));
jsonWrite(configSetup, ddnsNameS,  HTTP.arg(ddnsNameS));
jsonWrite(configSetup, ddnsPortS,  HTTP.arg(ddnsPortS));
  ip_wan();
  saveConfigSetup ();
  httpOkText();
}

// --------------------------------Запрос для синхронизации внешнего ip адреса с ddns
int ip_wan() {
  String ddns = jsonRead(configSetup, ddnsS);
  if (ddns != "") {
    getURL(ddns);
  }
}
