#ifdef mqttM
/*
    Для использования вкладки требуется добавить в заголовке скетча следуюший код
  #include <PubSubClient.h>
  WiFiClient wclient;
  PubSubClient client(wclient);
*/
void initMQTT() {

  if ( getOptions(messageS) != emptyS) { // Если нет связи с интернет не запускать
    //MQTT_Connecting();
    ts.add(tMQTT, 60000, [&](void*) {
      //Serial.println(",MQTT_Connecting");
      if (!client.connected()) MQTT_Connecting();
    }, nullptr, true);
    sCmd.addCommand(mqttS.c_str(), handle_mqtt);
    modulesReg(mqttS);
    sCmd.addCommand("wReg", widgetReg);
    sCmd.addCommand("sWidget", sendWidget);
  }
}

void  handleMQTT() {
  if (client.connected()) {
    if (order != "") {
      //Serial.println(order);
      String tmp = selectToMarker(order, ",");
      sCmd.readStr(tmp);
      order = deleteBeforeDelimiter(order, ",");
    }
    client.loop();

  }
  //else Serial.println("NOT mqtt");
}

// ------------------------------Установка параметров mqtt
void handle_mqtt() {
  String mqttServer = readArgsString();
  int mqttPort = readArgsInt();
  String mqttUser = readArgsString();
  String mqttPass = readArgsString();
  sendSetup(mqttServerS, mqttServer);
  sendSetup(mqttPortS, mqttPort);
  sendSetup(mqttUserS, mqttUser);
  sendSetup(mqttPassS, mqttPass);
  if (!client.connected()) MQTT_Connecting();
  saveConfigSetup ();
}
void MQTT_Connecting() {
  String mqtt_server = getSetup(mqttServerS);
  if ((mqtt_server != emptyS)) {
    client.set_server(mqtt_server, getSetupInt(mqttPortS));
    // подключаемся к MQTT серверу
    if (WiFi.status() == WL_CONNECTED) {
      if (!client.connected()) {
        if (client.connect(MQTT::Connect(chipID)
                           .set_auth(getSetup(mqttUserS), getSetup(mqttPassS)))) {
          client.set_callback(callback);
          client.subscribe(prefix);  // Для приема получения HELLOW и подтверждения связи
          client.subscribe(prefix + "/" + chipID + "/+/control"); // Подписываемся на топики control
          client.subscribe(prefix + "/ids"); // Подписываемся на топики ids
          //unsigned long timeSa = millis();
          loadnWidgets();
          //timeSa = millis() - timeSa;
          //Serial.println(timeSa);
        }
      }
    }
  }
}
void callback(const MQTT::Publish& pub) {
  String payload = pub.payload_string();
  //---------все что происходит при обновлении страницы iot manager на телефоне------
  if (pub.payload_string() == "HELLO" ) {
    //Serial.println("HELLO");
    loadnWidgets();
  }
  if (String(pub.topic()) == prefix + "/ids") ids = pub.payload_string();
  String topic_ = String(pub.topic());
  if (topic_.indexOf("control") > 0) {
    topic_ = deleteToMarkerLast(topic_, "/control");
    topic_ = selectToMarkerLast(topic_, "/");
    topic_ = topicToCom(topic_);
    topic_.replace(" ", " " + pub.payload_string() + " ");
    order += topic_ + ",";
    //Serial.println(order);
  }
}



// Читаем и отправляем виджеты на сервер
bool loadnWidgets() {
  uint8_t num = 0;
  String setW = configwidgets;
  while (setW.length() != 0) {
    String temp = selectToMarker (setW, ",");
    setW = deleteBeforeDelimiter(setW, ",");
    temp += " ";
    temp += num;
    sCmd.readStr("sWidget " + temp);
    num++;
  }
}

// Регистрируем список виджетов в configwidgets командой wReg
void widgetReg() {
  String nameW = readArgsString(); // Имя виджета
  String topicN = readArgsString(); // Топик
  String descrN = readArgsString(); // подпись
  //Serial.println(topicN);
  configwidgets += nameW + " ";
  configwidgets += topicN + " ";
  configwidgets += descrN + ",";
}

// Отправляем виджет на сервер командой sWidget
void sendWidget() {
  static String nameWp;
  String nameW = readArgsString(); // Имя виджета
  String topicN = readArgsString(); // Топик
  String descrN = readArgsString(); // подпись
  int num = readArgsInt();
  String prex = prefix + "/" + chipID;

  static String thing_config;
  if (nameWp != nameW) {
    //Serial.println(nameWp);
    thing_config = readFile("widgets/" + nameW + ".json", 6096);
    nameWp = nameW;
  }


  jsonWrite(thing_config, "page", getSetup(spaceS));
  jsonWrite(thing_config, "topic", prex + "/" + topicN);
  jsonWrite(thing_config, "id", num);
  jsonWrite(thing_config, "descr", descrN);
  //unsigned long timeSa = millis();
  sendMQTT("config", thing_config);
  //timeSa = millis() - timeSa;
  //Serial.println(timeSa);
  sendMQTTstatus(topicN, "status", getStatusInt(topicN));
}

// Отправляем данные на сервер
void sendMQTT(String topicN, String data) {
  topicN = prefix + "/" + chipID + "/" + topicN;

  client.publish(MQTT::Publish(topicN, data).set_qos(1));
}

//=======================================УПРАВЛЕНИЕ ВИДЖЕТАМИ======================================================================

void sendMQTTstatus(String topicN, String key1, String date1) {

  topicN = prefix + "/" + chipID + "/" + topicN + "/status";
  String t = "{}";
  jsonWrite(t, key1, date1);
  client.publish(MQTT::Publish(topicN, t).set_qos(1));

}

void sendMQTTstatus(String topicN, String key1, int date1) {

  topicN = prefix + "/" + chipID + "/" + topicN + "/status";
  String t = "{}";
  jsonWrite(t, key1, date1);
  client.publish(MQTT::Publish(topicN, t).set_qos(1));

}
#endif
#ifdef ddnsM
// --------------------- Включаем DDNS
void initDDNS() {
  if ( getOptions(messageS) != emptyS) { // Если нет связи с интернет не запускать
    //HTTPWAN = ESP8266WebServer (getSetupInt(ddnsPortS));
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
    ts.add(tDDNS, 600000, [&](void*) {
      ip_wan();
    }, nullptr, true);
    ip_wan();
    HTTPWAN.begin();
    sCmd.addCommand("ddns", handle_ddns);
    modulesReg(ddnsS);
  }
}

void httpwanOkText(String text) {
  HTTPWAN.send(200, "text/plain", text);
}

// ------------------------------Установка параметров ddns
void handle_ddns() {
  String ddnsName = readArgsString();
  int ddnsPort = readArgsInt();
  String ddns = readArgsString();
  sendSetup(ddnsS, ddns);
  sendSetup(ddnsPortS, ddnsPort);
  sendSetup(ddnsNameS, ddnsName);

  ip_wan();
  saveConfigSetup ();
}

// --------------------------------Запрос для синхронизации внешнего ip адреса с ddns
int ip_wan() {
  String ddns = getSetup(ddnsS);
  if (ddns != emptyS) {
    getURL(ddns);
  }
}
#endif
