void dht_init(){
   HTTP.on("/sensor.json", handle_sensor);               //
   dht.setup(DHTPIN); // data pin 2
   Serial.println("DHT");
   Serial.println(dht.getModel());
  }

void handle_sensor() {
  String root = "{}";  // Формировать строку для отправки в браузер json формат
  //{"SSDP":"SSDP-test","ssid":"home","password":"i12345678","ssidAP":"WiFi","passwordAP":"","ip":"192.168.0.101"}
  // Резервируем память для json обекта буфер может рости по мере необходимти, предпочтительно для ESP8266
  DynamicJsonBuffer jsonBuffer;
  //  вызовите парсер JSON через экземпляр jsonBuffer
  JsonObject& json = jsonBuffer.parseObject(root);
  // Заполняем поля json
  json["Temperature"] = dht.getTemperature();
  json["Humidity"] = dht.getHumidity();
  json["type"] = dht.getModel();
    // Помещаем созданный json в переменную root
  root="";
  json.printTo(root);
  HTTP.send(200, "text/json", root);
}

 /*
  String json = "\"Temperature\":\"";
  String temp = "";
  temp += dht.getTemperature();
  if (temp == "nan") {
    json += "hidden";
  } else {
    json += temp;
  }
  // Влажность
  json += "\",\"Humidity\":\"";
  String humi = "";
  humi += dht.getHumidity();
  if (humi == "nan") {
    json += "hidden";
  } else {
    json += humi;
  }

  HTTP.send(200, "text/json", "[{"+json+"\"}]");
}

*/
