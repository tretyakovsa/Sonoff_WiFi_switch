void dht_init(){
  HTTP.on("/sensortype", sensorType);      // Определить тип сенсора
    dht.begin();
  }

 void sensorType(){
    HTTP.send(200, "text/json", sensors);
  }


