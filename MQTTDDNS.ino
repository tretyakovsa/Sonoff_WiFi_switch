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
    if (stled == 1)   command = "relayon";
    if (stled == 0)   command = "relayoff";


  }

}

void initMQTT() {
  HTTP.on("/mqtt", handle_Set_MQTT);
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
        if (client.connect(MQTT::Connect(chipID)
                           .set_auth(jsonRead(configSetup, "mqttUser"), jsonRead(configSetup, "mqttPass")))) {
          client.set_callback(callback);
          client.subscribe(prefix);  // Для приема получения HELLOW и подтверждения связи
          client.subscribe(prefix + "/+/+/control"); // Подписываемся на топики control
          //client.subscribe("/" + chipID + "/RELE_1"); // подписываемся по топик с данными для светодиода
          loadnWidgets();
        } else {
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

//Установка параметров  http://192.168.0.101/mqtt?server=m13.cloudmqtt.com&port=15535&user=cxluynva&pass=4cje5WEkzqvR
void handle_Set_MQTT() {              //
  configSetup = jsonWrite(configSetup, "mqttServer",  HTTP.arg("server"));
  configSetup = jsonWrite(configSetup, "mqttPort",  HTTP.arg("port").toInt());
  configSetup = jsonWrite(configSetup, "mqttUser",  HTTP.arg("user"));
  configSetup = jsonWrite(configSetup, "mqttPass",  HTTP.arg("pass"));
  saveConfigSetup ();
  client.disconnect();
  MQTT_Pablush();
  HTTP.send(200, "text/plain", "OK");   // отправляем ответ о выполнении
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
      //Widgets["nWidgets"][i]["descr"] = jsonRead(configSetup, "SSDP");
      String thing_config = Widgets["nWidgets"][i].as<String>();
      //Serial.println(thing_config);
      client.publish(MQTT::Publish(prefix + "/" + chipID + "/config", thing_config).set_qos(1));
    }
  }
  return true;
}


// --------------------- Включаем DDNS
void initDDNS() {
  HTTPWAN = ESP8266WebServer (jsonReadtoInt(configSetup, "ddnsPort"));
  HTTP.on("/ddns", handle_ddns);               // Установка параметров ddns
  HTTPWAN.begin();
    // задача синхронизайия с сервером ddns каждые 10 минут
 ts.add(4, 600000, [&](void*) {
    ip_wan();
  }, nullptr, true);
  modulesReg("ddns");
}

// ------------------------------Установка параметров ddns
void handle_ddns() {
  configSetup = jsonWrite(configSetup, "ddns",  HTTP.arg("ddns"));
  configSetup = jsonWrite(configSetup, "ddnsName",  HTTP.arg("ddnsName"));
  configSetup = jsonWrite(configSetup, "ddnsPort",  HTTP.arg("ddnsPort"));
  ip_wan();
  saveConfigSetup ();
  HTTP.send(200, "text/plain", "OK");
}

// --------------------------------Запрос для синхронизации внешнего ip адреса с ddns
int ip_wan() {
  String ddns = jsonRead(configSetup, "ddns");
  if (ddns != "") {
    getURL(ddns);
  }
}




