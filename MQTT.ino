/*
    Для использования вкладки требуется добавить в заголовке скетча следуюший код
  #include <PubSubClient.h>
  WiFiClient wclient;
  PubSubClient client(wclient);
*/
void initMQTT() {
  HTTP.on("/mqtt", HTTP_GET, []() {
  sendSetup(mqttServerS,  HTTP.arg("server"));
  sendSetup(mqttPortS,  HTTP.arg("port").toInt());
  sendSetup(mqttUserS,  HTTP.arg("user"));
  sendSetup(mqttPassS,  HTTP.arg("pass"));
  saveConfigSetup ();
  client.disconnect();
  MQTT_Pablush();
    httpOkText();
  });
  modulesReg("mqtt");
  MQTT_Pablush();
}

void MQTT_Pablush() {
  String mqtt_server = getSetup(mqttServerS);
  if ((mqtt_server != "")) {

    client.set_server(mqtt_server, getSetupInt(mqttPortS));
    // подключаемся к MQTT серверу
    if (WiFi.status() == WL_CONNECTED) {
      if (!client.connected()) {
        //Serial.println(mqtt_server);
        if (client.connect(MQTT::Connect(chipID)
                           .set_auth(getSetup(mqttUserS), getSetup(mqttPassS)))) {
          client.set_callback(callback);
          client.subscribe(prefix);  // Для приема получения HELLOW и подтверждения связи
          client.subscribe(prefix + "/+/+/control"); // Подписываемся на топики control
          //client.subscribe( prefix + "/" + chipID + "/+/control"); // Подписываемся на топики control

          sendMQTT("test","work");
        } else {
        }
      }
    }
  }
}

void callback(const MQTT::Publish& pub)
{
  //Serial.print(pub.topic()); // выводим в сериал порт название топика
  //Serial.print(" => ");
  //Serial.print(pub.payload_string()); // выводим в сериал порт значение полученных данных
  //Serial.println();
  String payload = pub.payload_string();
  //Serial.println(payload);
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
      jsonWrite(thing_config, "topic",  prex+ jsonRead(thing_config, "topic"));
      jsonWrite(thing_config, "page", getSetup(spaceS));
      //Serial.println(thing_config);
      client.publish(MQTT::Publish(prex + "/config", thing_config).set_qos(1));
    }
  }
  return true;
}
 void sendMQTT(String topik, String data){
  topik = prefix + "/" + chipID + "/"+topik;
  //topik = prex+topik
   client.publish(MQTT::Publish(topik, data).set_qos(1));
  }

// --------------------- Включаем DDNS
void initDDNS() {
  HTTPWAN = ESP8266WebServer (getSetupInt(ddnsPortS));
  HTTP.on("/ddns", handle_ddns);               // Установка параметров ddns
  // ------------------Выполнение команды из запроса
  HTTPWAN.on("/cmd", HTTP_GET, []() {
    String com = HTTPWAN.arg("command");
    //sendStatus("test", com);
    sCmd.readStr(com);
    httpwanOkText(com);
  });
  HTTPWAN.on("/", HTTP_GET, []() {
    String str = getSetup(ddnsNameS);
    httpwanOkText(str);
  });
   HTTPWAN.onNotFound([]() {
   //Serial.println("TEST DDNS");
      HTTPWAN.send(404, "text/plain", "FileNotFound");
  });

    // задача синхронизайия с сервером ddns каждые 6 минут
 ts.add(9, 600000, [&](void*) {
    ip_wan();
  }, nullptr, true);
  ip_wan();
  HTTPWAN.begin();
  modulesReg(ddnsS);
}

void httpwanOkText(String text) {
  HTTPWAN.send(200, "text/plain", text);
}

// ------------------------------Установка параметров ddns
void handle_ddns() {
sendSetupArg(ddnsS);
sendSetupArg(ddnsNameS);
sendSetupArg(ddnsPortS);
  ip_wan();
  saveConfigSetup ();
  httpOkText();
}

// --------------------------------Запрос для синхронизации внешнего ip адреса с ddns
int ip_wan() {
  String ddns = getSetup(ddnsS);
  if (ddns != "") {
    getURL(ddns);
  }
}
