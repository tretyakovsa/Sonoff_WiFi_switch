/*
  * Для использования вкладки требуется добавить в заголовке скетча следуюший код
 #include <DHT.h>                     //https://github.com/markruys/arduino-DHT
 // DHT C автоматическим определением датчиков.Поддержка датчиков DHT11,DHT22, AM2302, RHT03.
 DHT dht;
 Нет обращений к глобальным переменным
 Объект ESP8266WebServer HTTP(80); должен быть инециализирован в заголовке
*/

void sensor_init(){
  OneWire oneWire(DHTPIN);
  DallasTemperature sensors(&oneWire);
 HTTP.on("/sensor.json", handle_sensor);
  pinMode(DHTPIN, INPUT_PULLUP);
 dht.setup(DHTPIN);
 delay(dht.getMinimumSamplingPeriod());
 String temp ="";
 temp += dht.getTemperature();
  if (temp == "nan"){
     Serial.println("No DHT");
     sensors.begin();
     sensors.requestTemperatures();
     Serial.println(sensors.getTempCByIndex(0));
    }
    else{
       modulesReg("temperature");
      Serial.println("DHT");
      Serial.println(temp);
      }

 HTTP.on("/analog.json", handle_analog);
 modulesReg("analog");
}

void handle_sensor() {
 float temp = dht.getTemperature();
 if (temp == 'NaN') {temp = 0;}
 HTTP.send(200, "text/json", graf(temp,10,5000,"Temperature"));
}

void handle_analog() {

 HTTP.send(200, "text/json", graf(analogRead(A0),30,3000,"Analog (ADC/A0)"));
}

String graf(int datas, int points,int refresh,String title ){
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
 root="";
 json.printTo(root);
 return root;
  }
