// ---------------------Инициализация Web сервера и сервисных функций
void initHTTP() {
  // Кэшировать файлы для быстрой работы
  HTTP.serveStatic("/css/", SPIFFS, "/css/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/js/", SPIFFS, "/js/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/img/", SPIFFS, "/img/", "max-age=31536000"); // кеширование на 1 год
  //HTTP.serveStatic("/lang/", SPIFFS, "/lang/", "max-age=31536000"); // кеширование на 1 год

  // --------------------Выдаем данные configOptions
  HTTP.on("/config.options.json", HTTP_GET, []() {
    FSInfo fs_info;
    SPIFFS.info(fs_info);
    sendOptions("totalBytes", fs_info.totalBytes);
    sendOptions("usedBytes", fs_info.usedBytes);
    sendOptions("blockSize", fs_info.blockSize);
    sendOptions("pageSize", fs_info.pageSize);
    sendOptions("maxOpenFiles", fs_info.maxOpenFiles);
    sendOptions("maxPathLength", fs_info.maxPathLength);

    httpOkJson(configOptions);
  });

  /*
    // --------------------Выдаем данные configOptions
    HTTP.on("/test.json", HTTP_GET, []() {
    String tmpJson = "{}";
    String par = stateA0S;
    String prifexFile = "";
    // Сделаем имя файла
    String fileName = GetDate();
    fileName = deleteBeforeDelimiter(fileName, " "); // удалим день недели
    fileName.replace(" ", ".");
    fileName.replace("..", "."); // Заменяем пробелы точками
    fileName = par + "/" + fileName + ".txt"; // Имя файла параметр в виде директории и дата
    fileName.toLowerCase(); //fileName = "san aug 31 2018"; Имя файла строчными буквами


    File configFile = SPIFFS.open("/" + fileName, "r"); // Открываем файл Для чтения
    // {"data":[],"points":"10","refresh":"1000"}
    String testJ ="{\"data\":[";
    if (configFile) {
      Serial.println(fileName);
      // Читаем префикс
      String sim;
      do {
        sim = char(configFile.read());
        prifexFile += sim;
      } while (sim != ":");
      int inter = selectToMarker(prifexFile, ",").toInt();
      prifexFile = deleteBeforeDelimiter(prifexFile, ",");
      String par = selectToMarker(prifexFile, ",");
      prifexFile = deleteBeforeDelimiter(prifexFile, ",");
      int dataSize = selectToMarker(prifexFile, ":").toInt();
      Serial.println(dataSize);
       Здесь код хак
        //first create a fixed buffer
        const int bufferSize = 6000;
        uint8_t _buffer[6000];

        //a little counter to know at which position we are in our buffer
        int bufferCounter = 0;

        int fileSize = 80000;

        //now send an empty string but with the header field Content-Length to let the browser know how much data it should expect
        server.sendHeader("Content-Length", (String)fileSize);
        server.send(200, "text/html", "");

        for(int i=0;i<fileSize;i++){
        _buffer[bufferCounter] = random(256); //write some random bytes into our buffer
        bufferCounter++;

        if(bufferCounter >= bufferSize){ //when the buffer is full...
          server.sendContent_P(_buffer, bufferCounter); //send the current buffer
          bufferCounter = 0; //reset the counter
        }

        }

        //send the rest bytes if there are some
        if(bufferCounter > 0){
        server.sendContent_P(_buffer, bufferCounter);
        bufferCounter = 0;
        }

        // -------------

      int test=0;
      while (configFile.available()) {
        int date;
        for (int d = 0; d < dataSize; d++) {
          date = configFile.read();
          date = (date << 8) + configFile.read();
        }
        testJ +=date;
        testJ +=",";
        test++;
        //Serial.print(test);
        //Serial.print("=");
        //Serial.println(date);
      }
      testJ +="0]}";
    }
    configFile.close();

    httpOkText(testJ);
    });
  */

  // --------------------Выдаем данные configJson
  HTTP.on("/config.live.json", HTTP_GET, []() {

    httpOkJson(configJson);
  });

  // --------------------Выдаем данные configwidgets
  HTTP.on("/config.widgets.json", HTTP_GET, []() {
    httpOkJson(configwidgets);
  });

  // -------------------Выдаем данные configSetup
  HTTP.on("/config.setup.json", HTTP_GET, []() {
    httpOkJson(configSetup);
  });
  // -------------------Выдаем данные regCommands
  HTTP.on("/command.json", HTTP_GET, []() {
    httpOkJson(regCommands);
  });

  // --------------------Узнать какие модули есть в устройстве
  HTTP.on("/modules.json", HTTP_GET, []() {

    httpOkJson(modules);
  });
  // --------------------Узнать какие модули есть в устройстве
  HTTP.on("/pulse.json", HTTP_GET, []() {
    httpOkJson(pulsList);
  });

  // -------------------Установка конфигурации
  HTTP.on("/configs", HTTP_GET, []() {
    String set = HTTP.arg("set");
    sendSetup(configsS, set);
    saveConfigSetup();
    String reqvest = "{\"action\": \"page.htm?configs&" + set + "\"}";
    httpOkText(reqvest);
  });

  // -------------------построение графика
  HTTP.on("/charts.json", HTTP_GET, []() {
    String pFile = HTTP.arg("data");
    String message = "{";
    if (pFile.indexOf(".") == -1) {
      for (uint8_t i = 0; i < HTTP.args(); i++) {
        //message += " " + HTTP.argName(i) + ": " + HTTP.arg(i) + "\n";
        message += "\"" + HTTP.argName(i) + "\":[";
        String key = getOptions(HTTP.arg(i));
        if (key != emptyS)  {
          message += key;
          key = emptyS;
        } else {
          key = getStatus(HTTP.arg(i));
          if (key != emptyS)  {
            message += key;
            key = emptyS;
          }
        }
        message += "],";
      }
    }
    message += "\"points\":\"10\",\"refresh\":\"1000\"}";

    httpOkText(message);
  });


  HTTP.on("/lang", HTTP_GET, []() {
    sendSetup(langS, HTTP.arg("set"));
    setupToOptions(langS);
    jsonWrite(modules, langS, getSetup(langS));
    saveConfigSetup();
    httpOkText();
  });
  // ------------------Выполнение команды из запроса
  HTTP.on("/cmd", HTTP_GET, []() {
    String com = HTTP.arg("command");
    //Serial.println(com);
    sendOptions("test", com);
    sCmd.readStr(com);
    httpOkText(statusS);
    //Serial.println(statusS);
  });
  // ------------------Выполнение голосовой команды
  HTTP.on("/voice", HTTP_GET, []() {
    String com = HTTP.arg("command");
    com.replace(" ", "_");
    sendOptions(voiceS, com);
    flag = sendStatus(voiceS, com);
    httpOkText(statusS);
  });
#ifdef macros
  // ------------------Список макросов
  HTTP.on("/macros.json", HTTP_GET, []() {
    String rn = "\n";
    String found = "if voice =";
    String temp  = Scenary;
    String macrosJson = "{\"macros\":[";
    do {
      String comm = selectToMarker (temp, rn);
      if (comm.indexOf(found) != -1)   {
        comm = deleteBeforeDelimiter(comm, found + " ");
        String out = "{}";
        jsonWrite(out, "command", comm);
        macrosJson += out + ",";
      }
      temp = deleteBeforeDelimiter(temp, rn);
      //yield();
    } while (temp.indexOf(rn) != -1);
    macrosJson = deleteToMarkerLast(macrosJson, ",");
    macrosJson += "]}";
    httpOkJson(macrosJson);
  });
#endif
  sCmd.addCommand(voiceS.c_str(), macros); //
  sendStatus(voiceS, "");
  commandsReg(voiceS);
}

void macros() {
  flag = sendStatus(voiceS, readArgsString());
}


void httpOkText() {
  HTTP.sendHeader("Access-Control-Allow-Origin", "*");
  HTTP.send(200, "text/plain", "Ok");
}
void httpOkText(String text) {
  HTTP.sendHeader("Access-Control-Allow-Origin", "*");
  HTTP.send(200, "text/plain", text);
}
void httpOkHtml(String text) {
  HTTP.sendHeader("Access-Control-Allow-Origin", "*");
  HTTP.send(200, "text/html", text);
}
void httpOkJson(String text) {
  HTTP.sendHeader("Access-Control-Allow-Origin", "*");
  HTTP.send(200, "application/json", text);
}
void http500send(String text) {
  HTTP.sendHeader("Access-Control-Allow-Origin", "*");
  HTTP.send(500, "text/plain", text);
}
void http404send() {
  HTTP.sendHeader("Access-Control-Allow-Origin", "*");
  HTTP.send(404, "text/plain", "FileNotFound");
}
// Инициализация FFS
void initFS() {
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
    }
    // Создаем список файлов каталога /lang
    Lang = FileList("/lang");
  }
  //HTTP страницы для работы с FFS
  //list directory
  HTTP.on("/list", HTTP_GET, handleFileList);
  //загрузка редактора editor
  HTTP.on("/edit", HTTP_GET, []() {
    if (!handleFileRead("/edit.htm")) http404send();//HTTP.send(404, "text/plain", "FileNotFound");
  });
  //Создание файла
  HTTP.on("/edit", HTTP_PUT, handleFileCreate);
  //Удаление файла
  HTTP.on("/edit", HTTP_DELETE, handleFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  HTTP.on("/edit", HTTP_POST, []() {
    //HTTP.send(200, "text/plain", emptyS);
    httpOkText(emptyS);
  }, handleFileUpload);
  //called when the url is not defined here
  //use it to load content from SPIFFS
  HTTP.onNotFound([]() {
    if (!handleFileRead(HTTP.uri()))
      http404send();//HTTP.send(404, "text/plain", "FileNotFound");
  });
  HTTP.on("/skins", HTTP_GET, []() {
    String set = HTTP.arg("set");
    //configJson = jsonWrite(configJson, "setIndex", set);
    jsonWrite(configSetup, "setIndex", set);
    saveConfigSetup();
    HTTP.send(307, "Temporary Redirect\r\nLocation: /\r\nConnection: Close\r\n", emptyS);
  });

}

// Здесь функции для работы с файловой системой
String getContentType(String filename) {
  if (HTTP.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return texthtmlS;
  else if (filename.endsWith(".html")) return texthtmlS;
  else if (filename.endsWith(".json")) return "application/json";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) {
  String setIndex =  jsonRead(configSetup, setIndexS);
  if (setIndex == emptyS) setIndex = "index.htm";
  if (path.endsWith("/")) path += setIndex;
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    //HTTP.header(String name);
    HTTP.sendHeader("Access-Control-Allow-Origin", "*");
    size_t sent = HTTP.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleFileUpload() {
  if (HTTP.uri() != "/edit") return;
  HTTPUpload& upload = HTTP.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    //DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile)
      fsUploadFile.close();
  }
}

void handleFileDelete() {
  if (HTTP.args() == 0) return http500send("BAD ARGS");// HTTP.send(500, "text/plain", "BAD ARGS");
  String path = HTTP.arg(0);
  if (path == "/")
    return http500send("BAD PATH");// HTTP.send(500, "text/plain", "BAD PATH");
  if (!SPIFFS.exists(path))
    return http404send();//HTTP.send(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  //HTTP.send(200, "text/plain", emptyS);
  httpOkText(emptyS);
  path = String();
}

void handleFileCreate() {
  if (HTTP.args() == 0)
    return http500send("BAD ARGS");//  HTTP.send(500, "text/plain", "BAD ARGS");
  String path = HTTP.arg(0);
  if (path == "/")
    return http500send("BAD PATH");//  HTTP.send(500, "text/plain", "BAD PATH");
  if (SPIFFS.exists(path))
    return http500send("FILE EXISTS");//  HTTP.send(500, "text/plain", "FILE EXISTS");
  File file = SPIFFS.open(path, "w");
  if (file)
    file.close();
  else
    return http500send("CREATE FAILED");// HTTP.send(500, "text/plain", "CREATE FAILED");
  //HTTP.send(200, "text/plain", emptyS);
  httpOkText(emptyS);
  path = String();

}

void handleFileList() {
  if (!HTTP.hasArg("dir")) {
    //HTTP.send(500, "text/plain", "BAD ARGS");
    http500send("BAD ARGS");//
    return;
  }
  String path = HTTP.arg("dir");
  //HTTP.send(200, "application/json", FileList(path));
  httpOkJson(FileList(path));
}

// Создаем список файлов каталога
String FileList(String path) {
  Dir dir = SPIFFS.openDir(path);
  path = String();
  String output = "[";
  while (dir.next()) {
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }
  output += "]";
  return output;
}


#ifdef webSocketM
void initWebSoket() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  sendOptions(webSocketS, 1);
}
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:  // Событие происходит при отключени клиента
      //Serial.println("web Socket disconnected");
      break;
    case WStype_CONNECTED: // Событие происходит при подключении клиента
      {
        //Serial.println("web Socket Connected");
        webSocket.sendTXT(num, configJson); // Отправим в всю строку с данными используя номер клиента он в num
      }
      break;
    case WStype_TEXT: // Событие происходит при получении данных текстового формата из webSocket
      if (length > 0) {
        String command = String((const char *)payload);
        String cmd = jsonRead(command, "cmd");
        if (cmd == "") {
          cmd = jsonRead(command, voiceS);
          sendOptions(voiceS, cmd);
          flag = sendStatus(voiceS, cmd);
        } else {
          sendOptions("test", cmd);
          sCmd.readStr(cmd);
        }

        //Serial.print("cmd=");
        //Serial.println(cmd);

      }

      //webSocket.sendTXT(num, "message here"); // Можно отправлять любое сообщение как строку по номеру соединения
      // webSocket.broadcastTXT("message here");
      break;
    case WStype_BIN:      // Событие происходит при получении бинарных данных из webSocket
      // webSocket.sendBIN(num, payload, length);
      break;
  }
}
// Отправка данных в Socket всем получателям
// Параметры Имя ключа, Данные, Предыдущее значение
void SoketData (String key, String data, String data_old)  {

  if(getOptions(webSocketS) != "") {
    if (data_old != data) {
      String broadcast = "{}";
      jsonWrite(broadcast, key, data);
      webSocket.broadcastTXT(broadcast);
      //Serial.println(getOptions(webSocketS));
    }
  }
}


#endif
