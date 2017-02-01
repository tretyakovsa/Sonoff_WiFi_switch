void dht_init(){
  HTTP.on("/sensortype", sensorType);      // Определить тип сенсора
   dht.setup(DHTPIN); // data pin 2
  }

 void sensorType(){
   // HTTP.send(200, "text/json", sensors);
  }


