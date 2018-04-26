// ---------------Инициализация модулей
void initCMD() {
  sCmd.addCommand("UART",       uart);
  //sCmd.addCommand("ONEWIRE",        initOneWire);
  sCmd.addCommand("NTP",        initNTP);
  sCmd.addCommand("Upgrade",    initUpgrade);
  sCmd.addCommand("DDNS",       initDDNS);
  sCmd.addCommand("A0",       initA0);
  sCmd.addCommand("TACH",       initTach);
  sCmd.addCommand("DHT",       initDHT);
  sCmd.addCommand("DS18B20",       initOneWire);
  //sCmd.addCommand("TIMERS",       initTimers);
  sCmd.addCommand("RELAY",       initRelay);
  sCmd.addCommand("POW",       initHLW8012);
  sCmd.addCommand("JALOUSIE",       initJalousie);
  sCmd.addCommand("MQTT",       initMQTT);
  sCmd.addCommand("RGB",       initRGB);
  sCmd.addCommand("SRGB",       initRGBSHIM);
  sCmd.addCommand("RF-RECEIVED",       rfReceived);
  sCmd.addCommand("RF-TRANSMITTER",     rfTransmitter);
  sCmd.addCommand("IR-RECEIVED",       irReceived);
  sCmd.addCommand("IR-TRANSMITTER",     irTransmitter);
  sCmd.addCommand("RF-LIVOLO",     rfLivolo);
  sCmd.addCommand("BUZZER",       initBuzzer);
  sCmd.addCommand("print",       printTest);
  sCmd.addCommand("FURNACE",       initFurnace);
  sCmd.addCommand("GET",       initGet);
  commandsReg("GET");
  sCmd.setDefaultHandler(unrecognized);
}

void unrecognized(const char *command) {
  //Serial.println("What?");
}

void initGet() {
  String urls = readArgsString();
  //Serial.println(urls);
  String answer = "";
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
