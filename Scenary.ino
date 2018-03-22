void initScenary() {
  loadScenary();
  sCmd.addCommand("if",       ifCommand);
  sCmd.addCommand("or",       orCommand);
  sCmd.addCommand("id",       idNot);
  sCmd.addCommand("then",       thenCommand);
  HTTP.on("/setscenary", HTTP_GET, []() {
    loadScenary();
    loadTimer();
    httpOkText();
  });
  loadScenary();
  loadTimer();
}

void loadScenary() {
  Scenary = readFile("scenary.save.txt", 4096);
  Scenary.replace("\r\n", "\n");
  Scenary.replace("\n\n", "\n");
  Scenary += "\n";
  //Serial.println(Scenary);
}

void loadTimer() {
  Timerset = readFile("timer.save.txt", 4096);
  Timerset.replace("\r\n", "\n");
  Timerset.replace("\n\n", "\n");
  Timerset += "\n";
  //Serial.println(Timerset);
}

// Ничего не делать если комманда id
void idNot() {}

void handleScenary() {
  yield();
  if (flag) {
    //Serial.println(goCommands(Scenary));
    goCommands(Scenary);
    //Serial.println(flag);
    testJson = configJson;
    flag = false;
  }
  if (flagT) {
    goCommands(Timerset);
    //Serial.println(goCommands(Timerset));
    //Serial.println("timer");
    flagT = false;
  }
}

// Разбор команды if
void ifCommand() {
    //Serial.println(thenOk);
    thenOk = false;
    orCommand();
  }

void orCommand() {
  String Name =  readArgsString();      // Какой параметр проверяем
  String Condition =  readArgsString(); // Операция
  String Volume =  readArgsString();    // Значение параметра
  String test = getStatus(Name);
  String testOld = jsonRead(testJson, Name);
  if (test != testOld) {
    //Serial.print("Name= ");
    //Serial.println(Name);
    //Serial.print("test= ");
    //Serial.println(test);
    //Serial.print("Volume= ");
    //Serial.println(Volume);
    //Serial.print("Condition= ");
    //Serial.println(Condition);
    if (Condition == "=") {
      //Serial.println("test=");
      if (Volume == test) thenOk = true;
    }
    if (Condition == "<") {
      if (Volume > test) thenOk = true;
    }
    if (Condition == ">") {
      if (Volume < test) thenOk = true;
    }
    if (Condition == "<=") {
      if (Volume >= test) thenOk = true;
    }
    if (Condition == ">=") {
      if (Volume <= test) thenOk = true;
    }
    if (Condition == "!=") {
      if (Volume != test) thenOk = true;
    }
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
    // Если это локальное устройство
    if (ssdp == test or test == "this") {
      //Serial.println("comm= ");
      //Serial.println(comm);
      sendStatus("test", comm);
      sCmd.readStr(comm);
    }
    else {
      //http://192.168.0.91/cmd?command=relay1
      String urls = "http://";
      String ip = jsonRead(ssdpList, test);
      urls += ip;
      urls += "/cmd?command=" + comm;
      urls.replace(" ", "%20");
      if (ip != "") {
        //Serial.println(urls);
        sendStatus("test", comm);
        getURL(urls);
      }

    }
  }
}
