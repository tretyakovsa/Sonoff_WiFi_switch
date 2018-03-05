/*
  // ----------------- OneWire
  void initOneWire() {
  uint8_t pin = readArgsInt();
  static uint16_t t = readArgsInt();
  Serial.println(t);
  static uint8_t averageFactor = readArgsInt();
  if (t < 500) t = 2000;
  //OneWire = new ow(pin);
  //DallasTemperature = new DS18B20(&OneWire);
  //DS18B20->begin();
  //DS18B20.requestTemperatures();
  //float ok = DS18B20.getTempCByIndex(0);
  //DS18B20.setResolution(12);
  //sendStatus(temperatureS, ok);

  ts.add(4, t, [&](void*) {
    //DS18B20.requestTemperatures();
    //flag = sendStatus("temperature", DS18B20.getTempCByIndex(0));
  }, nullptr, true);
  HTTP.on("/temperature.json", HTTP_GET, []() {
    //DS18B20.requestTemperatures();

    //String data = graf(DS18B20.getTempCByIndex(0), 10, t, "low:0");
    String data = graf(getStatusInt(temperatureS), 10, t, "low:0");
    httpOkJson(data);
  });
  modulesReg(temperatureS);
  }
*/
// -----------------  DHT
void initDHT() {
  uint8_t pin = readArgsInt();
  dht.setup(pin);
  delay(1000);
  static uint16_t t = readArgsInt();
  static uint16_t test = dht.getMinimumSamplingPeriod();
  if (t < test) t = test;
  Serial.println(t);
  String temp = "";
  temp += dht.getTemperature();
  if (temp != "nan") {
    sendStatus(temperatureS, temp);
    sendStatus(humidityS, dht.getHumidity());
    ts.add(5, test, [&](void*) {
      flag = sendStatus(temperatureS, dht.getTemperature());
      flag = sendStatus(humidityS, dht.getHumidity());
    }, nullptr, true);
    HTTP.on("/temperature.json", HTTP_GET, []() {
      String data = graf(getStatusInt(temperatureS), 10, t, "low:0");
      httpOkJson(data);
    });
    HTTP.on("/humidity.json", HTTP_GET, []() {
      String data = graf(getStatusInt(humidityS), 10, t, "low:0");
      httpOkJson(data);
    });
    modulesReg(temperatureS);
    modulesReg(humidityS);
  }
}

// -----------------  Кнопка
void initTach() {
  uint8_t pin = readArgsInt(); // первый аргумент pin
  String num = readArgsString(); // второй аргумент прификс реле 0 1 2
  uint16_t bDelay = readArgsInt(); // третий время подавления дребезга
  sendStatus(stateTachS + num, 0);
  buttons[num.toInt()].attach(pin);
  buttons[num.toInt()].interval(bDelay);
  but[num.toInt()] = true;
  boolean inv = readArgsInt(); // четвертый аргумент инверсия входа
  sendOptions("invTach" + num, inv);
  String m = readArgsString();
  if (m == "m") {
    sendOptions(movementTimeS, readArgsInt());
    sendStatus(stateMovementS, 0);
    modulesReg("movement");
    HTTP.on("/movement.json", HTTP_GET, []() {
      String data = graf(getStatusInt(stateMovementS), 10, 1000, "low:0");
      httpOkJson(data);
    });
    sCmd.addCommand("motion",     motionOn); //
    commandsReg("motion");
  }
}

void handleButtons() {
  static uint8_t num = 0;
  if (but[num]) {
    buttons[num].update();
    //if (buttons[num].fell() != getStatusInt(stateTachS + String(num, DEC))) {
    if (buttons[num].fell()) {
      flag = sendStatus(stateTachS + String(num, DEC), !getOptionsInt("invTach" + String(num)));
    }
    //if (buttons[num].rose() != getStatusInt(stateTachS + String(num, DEC))) {
    if (buttons[num].rose()) {
      flag = sendStatus(stateTachS + String(num, DEC), getOptionsInt("invTach" + String(num)));
    }
    //buttons[num].rose()
  }
  num++;
  //if (num == getOptionsInt(buttonNumS)) num = 0;
  if (num == 8) num = 0;
}

// -----------------  Движение

void motionOn() {
  uint16_t t = getOptionsInt(movementTimeS);
  motion.attach(t, motionOff);
  if (!getStatusInt(stateMovementS)) {
    flag = sendStatus(stateMovementS, 1);
  }
}
void motionOff() {
  motion.detach();
  if (getStatusInt(stateMovementS)) {
    flag = sendStatus(stateMovementS, 0);
  }
}

// -----------------  Аналоговый вход A0
void initA0() {
  static uint16_t t = readArgsInt();
  static uint8_t averageFactor = readArgsInt();
  if (t < 500) t = 1000;
  if (averageFactor == 0) averageFactor = 1023;
  analogRead(A0);
  sendStatus("stateA0", analogRead(A0));
  ts.add(3, t, [&](void*) {
    uint32_t a = 0;
    for (uint8_t i = 1; i <= 10; i++) {
      a += analogRead(A0);
    }
    a = a / 10;
    flag = sendStatus(stateA0S, a);
    //flag = sendStatus(stateA0S, map(a, 0, 1023, 0, averageFactor));
    //flag = sendStatus(stateA0S, map(analogRead(A0),0,1023,0,averageFactor));
  }, nullptr, true);
  HTTP.on("/analog.json", HTTP_GET, []() {
    String data = graf(getStatusInt(stateA0S), 10, t, "low:0");
    httpOkJson(data);
  });
  modulesReg("analog");
}

// ------------- Создание данных для графика
String graf(int datas, int points, int refresh, String options) {
  String root = "{}";  // Формировать строку для отправки в браузер json формат
  // {"data":[1],"points":"10","refresh":"1","title":"Analog"}
  // Резервируем память для json обекта буфер может рости по мере необходимти, предпочтительно для ESP8266
  DynamicJsonBuffer jsonBuffer;
  // вызовите парсер JSON через экземпляр jsonBuffer
  JsonObject& json = jsonBuffer.parseObject(root);
  // Заполняем поля json
  JsonArray& data = json.createNestedArray("data");
  data.add(datas);
  json["points"] = points;
  json["refresh"] = refresh;
  json["options"] = options;
  //"options":"low:0,showLine: false,showArea:true,showPoint:false",
  // Помещаем созданный json в переменную root
  root = "";
  json.printTo(root);
  return root;
}

// ----------------------Приемник ИK
void irReceived() {
  byte pin = readArgsInt();
  if (pin == 1 || pin == 3)  Serial.end();
  irReceiver = new IRrecv(pin);  // Create a new IRrecv object. Change to what ever pin you need etc.
  irReceiver->enableIRIn(); // Start the receiver
  // задача опрашивать RC код
  ts.add(6, 100, [&](void*) {
    handleIrReceiv();
  }, nullptr, true);
  sendStatus(irReceivedS, "ffffffff");
  modulesReg(irReceivedS);
}

void handleIrReceiv() {
  if (irReceiver->decode(&results)) {
    //serialPrintUint64(results.value, HEX);
    //Serial.println("");
    dump(&results);
    flag = sendStatus(irReceivedS, String((uint32_t) results.value, HEX));
    irReceiver->resume();  // Continue looking for IR codes after your finished dealing with the data.
  }

}

void dump(decode_results *results) {
  // Dumps out the decode_results structure.
  // Call this after IRrecv::decode()
  uint16_t count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
    sendOptions(irDecodeTypeS, "UNKNOWN");
  } else if (results->decode_type == NEC) {
    sendOptions(irDecodeTypeS, "NEC");
  } else if (results->decode_type == SONY) {
    sendOptions(irDecodeTypeS, "SONY");
  } else if (results->decode_type == RC5) {
    sendOptions(irDecodeTypeS, "RC5");
  } else if (results->decode_type == RC5X) {
    sendOptions(irDecodeTypeS, "RC5X");
  } else if (results->decode_type == RC6) {
    sendOptions(irDecodeTypeS, "RC6");
  } else if (results->decode_type == RCMM) {
    sendOptions(irDecodeTypeS, "RCMM");
  } else if (results->decode_type == PANASONIC) {
    sendOptions(irDecodeTypeS, "PANASONIC");
  } else if (results->decode_type == LG) {
    sendOptions(irDecodeTypeS, "LG");
  } else if (results->decode_type == JVC) {
    sendOptions(irDecodeTypeS, "JVC");
  } else if (results->decode_type == AIWA_RC_T501) {
    sendOptions(irDecodeTypeS, "AIWA_RC_T501");
  } else if (results->decode_type == WHYNTER) {
    sendOptions(irDecodeTypeS, "WHYNTER");
  } else if (results->decode_type == NIKAI) {
    sendOptions(irDecodeTypeS, "NIKAI");
  }
}

// ----------------------Приемник на 433мГ
void rfReceived() {
  byte pin = readArgsInt();
  if (pin == 1 || pin == 3)  Serial.end();
  mySwitch.enableReceive(pin);
  // задача опрашивать RC код
  ts.add(7, 5, [&](void*) {
    // handleRfReceiv();
  }, nullptr, true);
  sendStatus(rfReceivedS, 0);
  sendStatus(rfBitS, 0);
  sendStatus(rfProtocolS, 0);
  modulesReg(rfReceivedS);
}


void handleRfReceiv() {
  if (mySwitch.available()) {
    int value = mySwitch.getReceivedValue();
    if (value == 0) {
      sendStatus(rfReceivedS, 0);
      sendStatus(rfBitS, 0);
      sendStatus(rfProtocolS, 0);

    } else {
      uint32_t temp = mySwitch.getReceivedValue() ;
      flag = sendStatus(rfReceivedS, temp);
      temp = mySwitch.getReceivedBitlength();
      sendStatus(rfBitS, temp);
      temp = mySwitch.getReceivedProtocol();
      sendStatus(rfProtocolS, temp);
    }
    mySwitch.resetAvailable();
  }
}
