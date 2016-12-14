void dht_init(int pin){
  HTTP.on("/sensortype", sensorType);      // Определить тип сенсора
  DHT_Unified dht(pin, DHT11);
  dht.begin();
   uint32_t delayMS;
  Serial.println("DHTxx Unified Sensor Example");
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
    String dhtparm = "{";
      dhtparm += "\"Sensor\":\"";
      dhtparm += sensor.name;
      dhtparm += "\",";
      dhtparm += "\"DriverVer\":";
      dhtparm +=sensor.version;
      dhtparm += ",";
      dhtparm += "\"UniqueID\":";
      dhtparm +=sensor.sensor_id;
      dhtparm += ",";
      dhtparm += "\"MaxValue\":";
      dhtparm +=sensor.max_value;
      dhtparm += ",";
      dhtparm += "\"MinValue\":";
      dhtparm +=sensor.min_value;
      dhtparm += ",";
      dhtparm += "\"Resolution\":";
      dhtparm +=sensor.resolution;
      dhtparm += ",";
      dhtparm += "}";
      dht.humidity().getSensor(&sensor);
      dhtparm += "{";
      dhtparm += "\"Sensor\":\"";
      dhtparm += sensor.name;
      dhtparm += "\",";
      dhtparm += "\"DriverVer\":";
      dhtparm +=sensor.version;
      dhtparm += ",";
      dhtparm += "\"UniqueID\":";
      dhtparm +=sensor.sensor_id;
      dhtparm += ",";
      dhtparm += "\"MaxValue\":";
      dhtparm +=sensor.max_value;
      dhtparm += ",";
      dhtparm += "\"MinValue\":";
      dhtparm +=sensor.min_value;
      dhtparm += ",";
      dhtparm += "\"Resolution\":";
      dhtparm +=sensor.resolution;
      dhtparm += ",";
      dhtparm += "}";
      Serial.println(dhtparm);
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
  sensors=dhtparm;
  }

 void sensorType(){

    HTTP.send(200, "text/json", sensors);
  }
