// ---------------------Инициализация Web сервера и сервисных функций
void initHTTP() {
  // Кэшировать файлы для быстрой работы
  HTTP.serveStatic("/css/", SPIFFS, "/css/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/js/", SPIFFS, "/js/", "max-age=31536000"); // кеширование на 1 год
  HTTP.serveStatic("/img/", SPIFFS, "/img/", "max-age=31536000"); // кеширование на 1 год
  //HTTP.serveStatic("/lang/", SPIFFS, "/lang/", "max-age=31536000"); // кеширование на 1 год
  sendStatus("voice", "go");
  // --------------------Выдаем данные configOptions
  HTTP.on("/config.options.json", HTTP_GET, []() {
    FSInfo fs_info;
    SPIFFS.info(fs_info);
    sendOptions("totalBytes",fs_info.totalBytes);
    sendOptions("usedBytes",fs_info.usedBytes);
    sendOptions("blockSize",fs_info.blockSize);
    sendOptions("pageSize",fs_info.pageSize);
    sendOptions("maxOpenFiles",fs_info.maxOpenFiles);
    sendOptions("maxPathLength",fs_info.maxPathLength);
    httpOkJson(configOptions);
  });

    // --------------------Выдаем данные configJson
  HTTP.on("/config.live.json", HTTP_GET, []() {
    httpOkJson(configJson);
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

  // -------------------Установка конфигурации
  HTTP.on("/configs", HTTP_GET, []() {
    String set = HTTP.arg("set");
    sendSetup(configsS, set);
    saveConfigSetup();
    String reqvest = "{\"action\": \"page.htm?configs&" + set + "\"}";
    httpOkText(reqvest);
  });

  HTTP.on("/lang", HTTP_GET, []() {
    sendSetup(langS, HTTP.arg("set"));
    setupToOptions(langS);
    jsonWrite(modules, langS, getSetup(langS)); //???????????????????????????
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
    sendOptions("voice", com);
    flag = sendStatus("voice", com);
    httpOkText(statusS);
  });

}


void httpOkText() {
  HTTP.send(200, "text/plain", "Ok");
}
void httpOkText(String text) {
  HTTP.send(200, "text/plain", text);
}
void httpOkHtml(String text) {
  HTTP.send(200, "text/html", text);
}
void httpOkJson(String text) {
  HTTP.send(200, "application/json", text);
}
void http500send(String text){
  HTTP.send(500, "text/plain", text);
  }
  void http404send(){
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
    //HTTP.send(200, "text/plain", "");
    httpOkText("");
  }, handleFileUpload);
  //called when the url is not defined here
  //use it to load content from SPIFFS
  HTTP.onNotFound([]() {
    if (!handleFileRead(HTTP.uri()))
      http404send();//HTTP.send(404, "text/plain", "FileNotFound");
  });
  HTTP.on("/skins", HTTP_GET, []() {
    String set=HTTP.arg("set");
    //configJson = jsonWrite(configJson, "setIndex", set);
    jsonWrite(configSetup, "setIndex", set);
    saveConfigSetup();
    HTTP.send(307, "Temporary Redirect\r\nLocation: /\r\nConnection: Close\r\n", "");
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
  if (setIndex == "") setIndex = "index.htm";
  if (path.endsWith("/")) path += setIndex;
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
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
  //HTTP.send(200, "text/plain", "");
  httpOkText("");
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
  //HTTP.send(200, "text/plain", "");
  httpOkText("");
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


/*
// webSocket

void initWebSocket(){
 // start webSocket server
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            sendOptions(webSocketS,getOptionsInt(webSocketS)-1);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            sendOptions(webSocketS,getOptionsInt(webSocketS)+1);
            sendOptions(webSocketS, num);
            //USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // send message to client
            //webSocket.sendTXT(num, "Connected");
        }
            break;
        case WStype_TEXT:
            //USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);
      // send message to server
      //webSocket.sendTXT(num,"message here");
      //webSocket.broadcastTXT(configJson);

            break;
    }

}
*/
