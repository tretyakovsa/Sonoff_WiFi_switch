// ---------------Инициализация модулей
void initCMD() {

  sCmd.addCommand("UART",       uart);
  //sCmd.addCommand("ONEWIRE",        initOneWire);
  sCmd.addCommand("NTP",        initNTP);
  sCmd.addCommand("UPGRADE",    initUpgrade);
#ifdef ddnsM
  sCmd.addCommand("DDNS",       initDDNS);
#endif
  sCmd.addCommand("A0",       initA0);
  sCmd.addCommand("TACH",       initTach);
  sCmd.addCommand("DHT",       initDHT);
#ifdef Si7021
  sCmd.addCommand("SI7021",       initSi7021);
#endif
  sCmd.addCommand("DS18B20",       initOneWire);
  sCmd.addCommand("TIMERS",       initTimers);
#ifdef pinOutM
  sCmd.addCommand("PINOUT",       initPinOut);
  sCmd.addCommand("RELAY",       initRelay);
#endif
#ifdef DimmerM
  sCmd.addCommand("DIMMER",       initDimmer);
#endif
#ifdef POW
  sCmd.addCommand("POW",       initHLW8012);
#endif
#ifdef JalousieM
  sCmd.addCommand("JALOUSIE",       initJalousie);
#endif
#ifdef mqttM
  sCmd.addCommand("MQTT",       initMQTT);
#endif
#ifdef rgbM
  sCmd.addCommand("RGB",       initRGB);
#endif
#ifdef  rgbShimM
  sCmd.addCommand("SRGB",       initRGBSHIM);
#endif
  sCmd.addCommand("RF-RECEIVED",       rfReceived);
  sCmd.addCommand("RF-TRANSMITTER",     rfTransmitter);
  sCmd.addCommand("IR-RECEIVED",       irReceived);
#ifdef  irTransmitterM
  sCmd.addCommand("IR-TRANSMITTER",     irTransmitter);
#endif
  sCmd.addCommand("RF-LIVOLO",     rfLivolo);
  sCmd.addCommand("BUZZER",       initBuzzer);
  sCmd.addCommand("print",       printTest);
  sCmd.addCommand("ALARM",       alarmGet);
  sCmd.addCommand("//",       alarmComm);
  sCmd.addCommand("#",       alarmOff);
  sCmd.addCommand("GET",       initGet);
  sCmd.addCommand("ADMIN",       initAdmin);
  commandsReg("GET");
  sCmd.addCommand("reset",       initReset);
  commandsReg("reset");
  sCmd.setDefaultHandler(unrecognized);
}

void unrecognized(const char *command) {
  Serial.println("What?");
}
void initReset() {
  ESP.restart();
}
void alarmComm() {
  //Serial.println("Comment?");
}
void initAdmin() {
  modulesReg("admin");
}

void alarmOff() {
  //Serial.println("CommandOff?");
}

void alarmGet() {
  String volume = readArgsString();
  String high = readArgsString();
  String low = readArgsString();
  alarmLoad(volume, high, low);
}


void initGet() {
  String urls = readArgsString();
  if (urls.indexOf("/") == 0) urls = "http://"+getSetup(ipS)+urls;
  //Serial.println(urls);
  if (urls.indexOf("{{") != -1) {
    String param = urls;
    do {
      param = deleteBeforeDelimiter(param, "{{");
      String test = selectToMarker(param, "}}");
      param = deleteBeforeDelimiter(param, "}}");
      urls.replace("{{" + test + "}}", getStatus(test));
    } while (param.length() != 0);
  }
  //Serial.println(urls);
  String answer = emptyS;
  HTTPClient http;
  http.begin(urls); //HTTP
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    answer = http.getString();
  }
  http.end();
}

// По комманде print печатает аргумент для тастов
void printTest() {
  Serial.println("Test " + readArgsString());
}

// Настраивает Serial по команде sCmd.addCommand("Serial",       uart);
void uart() {
  Serial.end();
  Serial.begin(readArgsInt());
  delay(100);
  Serial.println();
}

// Читает аргументы из команд каждый слежующий вызов читает следующий аргумент возвращает String
String readArgsString() {
  String arg;
  arg = sCmd.next();
  //Serial.println(arg);
  if (arg == emptyS) arg = emptyS;
  return arg;
}
// Читает аргументы из команд каждый слежующий вызов читает следующий аргумент возвращает Int
int readArgsInt() {
  char *arg;
  arg = sCmd.next();
  if (arg != NULL) {
    return atoi(arg);
  }
  else {
    return 0;
  }
}
