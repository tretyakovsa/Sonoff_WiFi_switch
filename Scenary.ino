void initScenary() {
  sCmd.addCommand("if",  ifCommand);
  sCmd.addCommand("or",  orCommand);
  sCmd.addCommand("and",  andCommand);
  sCmd.addCommand("id",  idNot);
  sCmd.addCommand("then", thenCommand);
  HTTP.on("/setscenary", HTTP_GET, []() {
    loadScenary();
    //String alarmSet = "ALARM " + stateA0S + " " + highalarmA0S + " " + lowalarmA0S;
    //sCmd.readStr(alarmSet);
    //alarmSet = "ALARM " + stateA0S + " " + highalarmA0S + " " + lowalarmA0S;
    alarmLoadModules();
    loadTimer();
    httpOkText();
  });
  loadScenary();
}
void loadScenary() {
  Scenary = readFile(ScenaryS, 4096);
  Scenary.replace("\r\n", "\n");
  Scenary.replace("\n\n", "\n");
  Scenary += "\n";
  //writeFile("test.text",Scenary);

}
// Ничего не делать если комманда id
void idNot() {}

void handleScenary() {
  yield();
  if (flag) { // если произошло изменение в данных config.live.json
  //Serial.println(flag);
    //addFileString("events.txt",configJson+"\r\n");
    goCommands(Scenary); // Делаем разбор сценариев
    webSocket.broadcastTXT(configJson);
    //Serial.println(configJson);
    sendStatus("voice", emptyS);
    flag = false;
  }
}
// Разбор команды if
void ifCommand() {
  thenOk = false; // сброс признака
  orCommand();
}
void andCommand() {
  String Name =  readArgsString();      // Какой параметр проверяем
  String Condition =  readArgsString(); // Операция
  String Volume =  readArgsString();    // Значение параметра
  String test = getStatus(Name);        // получить текущее значение параметра

//    Serial.print(Name);
//    Serial.print("=");
//    Serial.println("and");

  if (thenOk) {
    thenOk = false; // сброс признака
    testCommand(Volume, Condition, test);
  }
}

void orCommand() {
  String Name =  readArgsString();      // Какой параметр проверяем
  String Condition =  readArgsString(); // Операция
  String Volume =  readArgsString();    // Значение параметра
  String test = getStatus(Name);        // получить текущее значение параметра

//    Serial.print(Name);
//    Serial.print("=");
//    Serial.println("or");

  testCommand(Volume, Condition, test);
}

void testCommand(String Volume, String Condition, String test) {
  // последовательно проверяем параметр на соответствие операции сравнения
  // и поднимаем признак исполнения then
  if (Condition == "=") {
    if (Volume == test) thenOk = true;
  }
  if (Condition == ">") {
    if (Volume.toFloat() < test.toFloat()) thenOk = true;
  }
  if (Condition == "<") {
    if (Volume.toFloat() > test.toFloat()) thenOk = true;
  }
  if (Condition == "<=") {
    if (Volume.toFloat() >= test.toFloat()) thenOk = true;
  }
  if (Condition == ">=") {
    if (Volume.toFloat() <= test.toFloat()) thenOk = true;
  }
  if (Condition == "!=") {
    if (Volume != test) thenOk = true;
  }
}


// Выполнение then
void thenCommand() {
  if (thenOk) {
    String ssdp = jsonRead(configSetup, ssdpS);
    String test = readArgsString();
    String comm = readArgsString();
    comm += " " + readArgsString();
    comm += " " + readArgsString();
    comm += " " + readArgsString();
    comm += " " + readArgsString();
    comm += " " + readArgsString();
    comm += " " + readArgsString();
    // Если это локальное устройство
    if (ssdp == test or test == "this") {
//            Serial.println("comm= ");
//            Serial.println(comm);
      sendOptions("test", comm);
      sCmd.readStr(comm);
    }
    else {
      //       Serial.println("comm= ");
      //      Serial.println(comm);
      //http://192.168.0.91/cmd?command=relay on 1
      String urls = "http://";
      String ip = jsonRead(ssdpList, test);
      urls += ip;
      urls += "/cmd?command=" + comm;
      urls.replace(" ", "%20");
      if (ip != emptyS) {
        sendOptions("test", comm);
        getURL(urls);
      }
    }
  }
}
