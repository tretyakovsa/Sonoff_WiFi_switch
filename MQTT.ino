/*
    Для использования вкладки требуется добавить в заголовке скетча следуюший код
  #include <PubSubClient.h>
  Глобальные переменные
  //Mqtt
  String mqtt_server = "cloudmqtt.com"; // Имя сервера MQTT
  int mqtt_port = 15535; // Порт для подключения к серверу MQTT
  String mqtt_user = ""; // Логи от сервер
  String mqtt_pass =""; // Пароль от сервера
  bool LedState = false;
  int tm = 0;
  float temp = 0;
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
  if (String(pub.topic()) == "/" + chipID + "/RELE_1") // проверяем из нужного ли нам топика пришли данные
  {
    int stled = payload.toInt(); // преобразуем полученные данные в тип integer
    Serial.println(stled);
    if (stled != state0) {
      task = 4;
      chaing = 1;
    }
    //Serial.println(stled);
  }

  if (String(pub.topic()) == prefix + "/"+chipID + "/RELE_1_not/control") // проверяем из нужного ли нам топика пришли данные
   {
    int stled = payload.toInt(); // преобразуем полученные данные в тип integer
    if (stled != state0) {
      task = 4;
      chaing = 1;
    }
    //Serial.println(stled);
  }

}

void MQTT_init() {
  Serial.println("MQTT");
  mqtt_ConfigJSON();
  pinMode(LED_PIN, OUTPUT);
  HTTP.on("/mqtt", handle_Set_MQTT);
  modulesReg("mqtt");
  MQTT_Pablush();
}

void MQTT_Pablush() {
  client.set_server(mqtt_server, mqtt_port);
  // подключаемся к MQTT серверу
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      Serial.println("Connecting to MQTT server");
      if (client.connect(MQTT::Connect(chipID)
                         .set_auth(mqtt_user, mqtt_pass))) {
        Serial.println("Connected to MQTT server");
        client.set_callback(callback);
        client.subscribe(prefix);  // for receiving HELLO messages and handshaking
        client.subscribe(prefix + "/+/+/control"); // for receiving GPIO control messages
        client.subscribe("/" + chipID + "/RELE_1"); // подписываемся по топик с данными для светодиода
    //client.subscribe(prefix + prefix + "/"+chipID + "/+/+/status"); // for receiving GPIO control messages
        //client.subscribe(prefix + "/"+chipID + "/RELE_1_not/control"); // подписываемся по топик с данными для светодиода
        loadnWidgets();
        } else {
        Serial.println("Could not connect to MQTT server");
      }
    }
  }
}

void  handleMQTT() {
  if (client.connected()) client.loop();
}

//Установка параметров  http://192.168.0.101/mqtt?server=m13.cloudmqtt.com&port=15535&user=cxluynva&pass=4cje5WEkzqvR
void handle_Set_MQTT() {              //
  mqtt_server = HTTP.arg("server");         // Получаем значение server из запроса сохраняем в глобальной переменной
  mqtt_port = HTTP.arg("port").toInt(); // Получаем значение port из запроса сохраняем в глобальной переменной
  mqtt_user = HTTP.arg("user"); // Получаем значение user из запроса сохраняем в глобальной переменной
  mqtt_pass = HTTP.arg("pass"); // Получаем значение pass из запроса сохраняем в глобальной переменной
  mqtt_ConfigJSON();
  saveConfig();                         // Функция сохранения данных во Flash
  client.disconnect();
  MQTT_Pablush();
  HTTP.send(200, "text/plain", "OK");   // отправляем ответ о выполнении
}

void mqtt_ConfigJSON() {
  // Резервируем память для json обекта буфер может рости по мере необходимти, предпочтительно для ESP8266
  DynamicJsonBuffer jsonBuffer;
  //  вызовите парсер JSON через экземпляр jsonBuffer
  JsonObject& json = jsonBuffer.parseObject(jsonConfig);
  // Заполняем поля json

  json["mqttServer"] = mqtt_server;
  json["mqttPort"] = mqtt_port;
  json["mqttUser"] = mqtt_user;
  json["mqttPass"] = mqtt_pass;

  // Помещаем созданный json в переменную root
  jsonConfig = "";
  json.printTo(jsonConfig);
  Serial.println(jsonConfig);
  }

// Читаем и отправляем виджеты на сервер
bool loadnWidgets() {
  File WidgetsFile = SPIFFS.open("/config.widgets.json", "r");
  if (!WidgetsFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = WidgetsFile.size();
  String Widgetsset = "";
  if (size > 4096) {
    Serial.println("Config file size is too large");
    WidgetsFile.close();
    return false;
  }
  // загружаем файл конфигурации в глобальную переменную
  Widgetsset = WidgetsFile.readString();
  WidgetsFile.close();
  DynamicJsonBuffer jsonBuffer;
  JsonObject& Widgets = jsonBuffer.parseObject(Widgetsset);
  JsonArray& WidgetsArray = Widgets["nWidgets"].asArray();
  int j = WidgetsArray.size();
  if (j != 0) {
    for (int i = 0; i <= j - 1; i++) {
      Widgets["nWidgets"][i]["topic"] = prefix + "/" + chipID + Widgets["nWidgets"][i]["topic"].as<String>();
      Widgets["nWidgets"][i]["descr"] = ssdpName;
      String thing_config = Widgets["nWidgets"][i].as<String>();
      Serial.println(thing_config);
      client.publish(MQTT::Publish(prefix + "/" + chipID + "/config", thing_config).set_qos(1));
    }
  }
  return true;
}

