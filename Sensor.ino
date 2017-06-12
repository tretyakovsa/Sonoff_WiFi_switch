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
 HTTP.send(200, "text/json", graf(temp,10,5000));
}

void handle_analog() {

 HTTP.send(200, "text/json", graf(analogRead(A0),30,3000));
}



  void Movement_init() {
  HTTP.on("/pir", handle_pir);        // Установить время на которое будет срабатывать датчик движения
  modulesReg("movement");
}

// Установить время на которое будет срабатывать датчик движения
void handle_pir() {
  pirTime = HTTP.arg("t").toInt();
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}
