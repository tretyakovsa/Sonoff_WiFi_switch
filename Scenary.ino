void initScenary() {
  loadScenary();
  sCmd.addCommand("if",       ifCommand);
  sCmd.addCommand("id",       idNot);
  sCmd.addCommand("then",       thenCommand);
  HTTP.on("/setscenary", HTTP_GET, []() {
    loadScenary();
    HTTP.send(200, "text/plain", "Ok");
  });
}

void loadScenary(){
  Scenary = readFile("scenary.save.txt", 4096);
  Scenary.replace("\r\n", "\n");
  Scenary.replace("\n\n", "\n");
  Scenary += "\n";
  }

// Ничего не делать если комманда id
void idNot() {}

void handleScenary() {
  if (flag) {
    goCommands(Scenary);
    //Serial.println(goCommands(Scenary));
    flag = false;
  }
}

// Разбор команды if
void ifCommand() {
  thenOk = false;
  String Name =  readArgsString();
  String Condition =  readArgsString();
  String Volume =  readArgsString();
  if (Condition == "=") {
    if (Volume == getStatus(Name)) thenOk = true;
  }
  if (Condition == "<") {
    if (Volume > getStatus(Name)) thenOk = true;
  }
  if (Condition == ">") {
    if (Volume < getStatus(Name)) thenOk = true;
  }
  if (Condition == "<=") {
    if (Volume >= getStatus(Name)) thenOk = true;
  }
  if (Condition == ">=") {
    if (Volume <= getStatus(Name)) thenOk = true;
  }
  if (Condition == "!=") {
    if (Volume != getStatus(Name)) thenOk = true;
  }
}
// Выполнение then
void thenCommand() {
  if (thenOk) {
    String ssdp = jsonRead(configSetup, "SSDP");
    String test = readArgsString();
    String comm = readArgsString();
    // Если это локальное устройство
    if (ssdp == test ) {
      sCmd.readStr(comm);
    }
    else {
      //http://192.168.0.91/cmd?command=relay1
      String urls = "http://";
      String ip = jsonRead(ssdpList, test);
      urls += ip;
      urls += "/cmd?command=" + comm;
      if (ip != "") {
        //Serial.println(urls);
        getURL(urls);
      }

    }
  }
}
