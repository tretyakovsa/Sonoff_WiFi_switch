void dht_init(){
 HTTP.on("/sensor.json", handle_sensor);
 dht.setup(DHTPIN);
 Serial.println("DHT");
 Serial.println(dht.getModel());
}

void handle_sensor() {
 String root = "{}";  // Формировать строку для отправки в браузер json формат
 //{"SSDP":"SSDP-test","ssid":"home","ssidPass":"i12345678","ssidAP":"WiFi","ssidApPass":"","ip":"192.168.0.101"}
 // Резервируем память для json обекта буфер может рости по мере необходимти, предпочтительно для ESP8266
 DynamicJsonBuffer jsonBuffer;
 //  вызовите парсер JSON через экземпляр jsonBuffer
 JsonObject& json = jsonBuffer.parseObject(root);
 // Заполняем поля json
 JsonArray& data = json.createNestedArray("data");
 data.add(dht.getTemperature());  // 6 is the number of decimals to print
 JsonArray& data2 = json.createNestedArray("data2");
 data2.add(dht.getHumidity());  // 6 is the number of decimals to print
 json["type"] = dht.getModel();
 json["points"] = "10";
 json["refresh"] = "5";
 json["title"] = "Temperature";
 // Помещаем созданный json в переменную root
 root="";
 json.printTo(root);
 HTTP.send(200, "text/json", root);
}
