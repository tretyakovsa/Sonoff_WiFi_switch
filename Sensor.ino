 /*
  * Для использования вкладки требуется добавить в заголовке скетча следуюший код
 #include <DHT.h>                     //https://github.com/markruys/arduino-DHT
 // DHT C автоматическим определением датчиков.Поддержка датчиков DHT11,DHT22, AM2302, RHT03.
 DHT dht;
 Нет обращений к глобальным переменным
 Объект ESP8266WebServer HTTP(80); должен быть инециализирован в заголовке
*/

void sensor_init(){
  HTTP.on("/sensor.json", handle_sensor);
 HTTP.on("/analog.json", handle_analog);
 dht.setup(DHTPIN);
 }

void handle_sensor() {
 String root = "{}";  // Формировать строку для отправки в браузер json формат
 //{"data":[0.00],"data2":[0.00],"type":true,"points":"10","refresh":"5","title":"Temperature"}
 // Резервируем память для json обекта буфер может рости по мере необходимти, предпочтительно для ESP8266
 DynamicJsonBuffer jsonBuffer;
 //  вызовите парсер JSON через экземпляр jsonBuffer
 JsonObject& json = jsonBuffer.parseObject(root);
 // Заполняем поля json
 JsonArray& data = json.createNestedArray("data");
 float temp = dht.getTemperature();
 if (temp) {temp = 0;}
 data.add(temp);  // 6 is the number of decimals to print
 JsonArray& data2 = json.createNestedArray("data2");
 temp = dht.getHumidity();
 if (temp) {temp = 0;}
 data2.add(temp);  // 6 is the number of decimals to print
 json["type"] = dht.getModel();
 json["points"] = "10";
 json["refresh"] = "5";
 json["title"] = "Temperature";
 // Помещаем созданный json в переменную root
 root="";
 json.printTo(root);
 HTTP.send(200, "text/json", root);
}

void handle_analog() {
 String root = "{}";  // Формировать строку для отправки в браузер json формат
 //{"data":[1],"points":"10","refresh":"1","title":"Analog"}
 // Резервируем память для json обекта буфер может рости по мере необходимти, предпочтительно для ESP8266
 DynamicJsonBuffer jsonBuffer;
 //  вызовите парсер JSON через экземпляр jsonBuffer
 JsonObject& json = jsonBuffer.parseObject(root);
 // Заполняем поля json
 JsonArray& data = json.createNestedArray("data");
 data.add(analogRead(A0));
 json["points"] = "10";
 json["refresh"] = "1";
 json["title"] = "Analog";
 // Помещаем созданный json в переменную root
 root="";
 json.printTo(root);
 HTTP.send(200, "text/json", root);
}

