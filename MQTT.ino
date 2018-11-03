/*
    Для использования вкладки требуется добавить в заголовке скетча следуюший код
  #include <PubSubClient.h>
  WiFiClient wclient;
  PubSubClient client(wclient);
*/
void initMQTT() {
  if ( getStatus(messageS)!=emptyS){ // Если нет связи с интернет не запускать
  MQTT_Connecting();
  ts.add(2, 60000, [&](void*) {
    if (!client.connected()) MQTT_Connecting();
  }, nullptr, true);
   sCmd.addCommand("mqtt", handle_mqtt);
   modulesReg("mqtt");
  }
}

void  handleMQTT() {
  if (client.connected()) client.loop();
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
          Serial.println("Connected to MQTT server");
          client.set_callback(callback);
          client.subscribe(prefix);  // Для приема получения HELLOW и подтверждения связи
          client.subscribe(prefix + "/"+chipID+"/+/control"); // Подписываемся на топики control
          client.subscribe(prefix + "/ids"); // Подписываемся на топики ids
          sendMQTT("test", "work");
          loadnWidgets();
          //sendWidget("toggle","stateRelay1");
          //sendWidget("toggle","stateRelay2");
        } else {
          Serial.println("Could not connect to MQTT server");
        }
      }
    }
  }
}

void callback(const MQTT::Publish& pub)
{
  Serial.print(pub.topic());
  Serial.print(" => ");
  Serial.print(pub.payload_string());
  Serial.println();
  String payload = pub.payload_string();
  //---------все что происходит при обновлении страницы iot manager на телефоне------
  if (pub.payload_string() == "HELLO" ) {

loadnWidgets();

    sendSTATUS("voice", "speech", "привет, в вашем доме ничего опасного не обнаружено, температура в комнате плюс 28 градусов, на улице плюс 25");
  }

  if (String(pub.topic()) == prefix + "/ids") ids = pub.payload_string();


  String topic_ = String(pub.topic());
  if (topic_.indexOf("control") > 0)
  {
    topic_ = deleteToMarkerLast(topic_, "/control");
    topic_ = selectToMarkerLast(topic_, "/");

    String t = topic_.substring(3);
    topic_.replace(t, " " + t);
    topic_ += " " + pub.payload_string();
    order += topic_ + ",";
    Serial.println(order);
  }
}

// Читаем и отправляем виджеты на сервер
bool loadnWidgets() {
  uint8_t num =0;
  String setW = configwidgets;
  while(setW.length()!=0){
 String temp = selectToMarker (setW, ",");
 setW=deleteBeforeDelimiter(setW, ",");
 Serial.println(temp);
 sendWidget(selectToMarker(temp, ":"),selectToMarkerLast(temp, ":"),num);
 num++;
}
}


void widgetReg(String nameW, String topicN){
  configwidgets +=nameW+":";
   configwidgets +=topicN+",";
  }


void sendWidget(String nameW, String topicN, uint8_t num) {

  String prex = prefix + "/" + chipID;
  String thing_config = readFile("widgets/widgets."+nameW+".json", 6096);
  jsonWrite(thing_config, "page", getSetup(spaceS));
  jsonWrite(thing_config, "topic", prex + "/"+topicN);
  jsonWrite(thing_config, "id", num);
  sendMQTT("config", thing_config);

  }

void sendMQTT(String topicN, String data) {
  topicN = prefix + "/" + chipID + "/" + topicN;

  client.publish(MQTT::Publish(topicN, data).set_qos(1));
}

//=======================================УПРАВЛЕНИЕ ВИДЖЕТАМИ======================================================================

void sendSTATUS(String topicN, String key1, String date1) {
  yield();
  topicN = prefix + "/" + chipID + "/" + topicN + "/status";
  String t = "{}";
  jsonWrite(t, key1, date1);
  client.publish(MQTT::Publish(topicN, t).set_qos(1));
  yield();
}

void sendSTATUS(String topicN, String key1, String date1, String key2, String date2) {
  yield();
  topicN = prefix + "/" + chipID + "/" + topicN + "/status";
  String t = "{}";
  jsonWrite(t, key1, date1);
  jsonWrite(t, key2, date2);
  client.publish(MQTT::Publish(topicN, t).set_qos(1));
  yield();
}

void sendCONFIG(String topik, String widgetConfig, String key, String date) {
  yield();
  topik = prefix + "/" + chipID + "/" + topik + "/status";
  String outer = "{\"widgetConfig\":";
  String inner = "{\"";
  inner = inner + key;
  inner = inner + "\":\"";
  inner = inner + date;
  inner = inner + "\"";
  inner = inner + "}}";
  String t = outer + inner;
  //Serial.println(t);
  client.publish(MQTT::Publish(topik, t).set_qos(1));
  yield();
}

// --------------------- Включаем DDNS
void initDDNS() {
    if ( getStatus(messageS)!=emptyS){ // Если нет связи с интернет не запускать
    HTTPWAN = ESP8266WebServer (getSetupInt(ddnsPortS));
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
    ts.add(10, 600000, [&](void*) {
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
