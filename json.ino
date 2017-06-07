// Создание данных для графика
String graf(int datas, int points, int refresh, String title ) {
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
  json["title"] = title;
  // Помещаем созданный json в переменную root
  root = "";
  json.printTo(root);
  return root;
}



// /json?file=test.json&id=module&search=relay
// Фильтр на json фаил
String jsonFilter(String fileName, String column, String value) {
  boolean nots = true;
  if (value.substring(0, 1) == "!") {
    value = value.substring(1);
    Serial.println(value);
    nots = false;
  }
  File jsonFile = SPIFFS.open("/" + fileName, "r");
  if (!jsonFile) {
    return "Failed to open config file";
  }

  size_t size = jsonFile.size();

  Timerset = "";
  if (size > 1024) {
    Serial.println("Config file size is too large");
    jsonFile.close();
    return "Config file size is too large";
  }
  // загружаем файл конфигурации в глобальную переменную
  String jsonString = jsonFile.readString();
  jsonFile.close();
  DynamicJsonBuffer jsonBuffer;
  JsonArray& nestedArray = jsonBuffer.parseArray(jsonString);
  int j = nestedArray.size();
  Serial.println(value + " " + column);
  jsonString = "[";
  if (j != 0) {
    for (int i = 0; i <= j - 1; i++) {
      boolean pFind = (value == nestedArray[i][column].as<String>());
      if (pFind) {
        //nestedArray.removeAt(i);
        jsonString += nestedArray[i].as<String>();
        if (i <= j - 1) jsonString += ",";
        //i--;
      }
    }
  }

  jsonString += "]";



  //nestedArray.printTo(jsonString);
  return jsonString;
}

String jsonArray(String files, String nameArray) {
  File jsonFile = SPIFFS.open("/" + files, "r");
  if (!jsonFile) {
    return "Failed to open config file";
  }
  size_t size = jsonFile.size();
  Timerset = "";
  if (size > 4096) {
    jsonFile.close();
    return "Config file size is too large";
  }
  // загружаем файл в объект
  String jsonAll = jsonFile.readString();
  jsonFile.close();
  DynamicJsonBuffer jsonBuffer;
  JsonObject& All = jsonBuffer.parseObject(jsonAll);
  JsonArray& nestedArray = All[nameArray].asArray();
  int j = nestedArray.size();
  jsonAll="";
  nestedArray.printTo(jsonAll);
  return jsonAll;
}
