// relay":[1,1,0]
void relay_init() {
  pinMode(RELE1_PIN, OUTPUT);
  HTTP.on("/sonoff", handle_sonoff);               // Узнать какие модули есть в устройстве
  modulesReg("relay");
}

void handle_sonoff(){
   task = 1;
   chaing = 1;
  HTTP.send(200, "text/json", "Ok");
  }

void handleRelay() {
  if (chaing) {
    noInterrupts();
    switch (task) {
      case 1:
        state0 = !state0;
        digitalWrite(RELE1_PIN, state0);
        client.publish(MQTT::Publish(chipID + "/RELE_1",  String(state0)).set_retain(1));
        //client.publish(prefix + "/vorota1", "0", true);
        chaing = 0;
        break;
      case 4:
        state0 = !state0;
        digitalWrite(RELE1_PIN, state0);

        chaing = 0;
        break;


    }

  }
}
