void relay_init() {
  pinMode(RELE1_PIN, OUTPUT);
  sCmd.addCommand("relayOn",     relayOn);
  sCmd.addCommand("relayOff",    relayOff);
  sCmd.addCommand("relayNot",    relayNot);
  modulesReg("relay");
}

void relayOn() {
  state0 = 1;
  digitalWrite(RELE1_PIN, state0);
  client.publish(MQTT::Publish(prefix + "/"+chipID + "/RELE_1_not/status",    "{\"status\":"+String(state0)+"}").set_retain(1));
}
void relayOff() {
  state0 = 0;
  digitalWrite(RELE1_PIN, state0);
  client.publish(MQTT::Publish(prefix + "/"+chipID + "/RELE_1_not/status",    "{\"status\":"+String(state0)+"}").set_retain(1));
}
void relayNot() {
  state0 = !state0;
  digitalWrite(RELE1_PIN, state0);
  client.publish(MQTT::Publish(prefix + "/"+chipID + "/RELE_1_not/status",    "{\"status\":"+String(state0)+"}").set_retain(1));
}


/*
  void handleRelay() {
  if (chaing) {
    noInterrupts();
    switch (task) {
      case 1:
        state0 = !state0;
        digitalWrite(RELE1_PIN, state0);
  //        digitalWrite(RELE2_PIN, state0);
  //        digitalWrite(RELE3_PIN, state0);
        //client.publish(MQTT::Publish("/" + chipID + "/RELE_1",  String(state0)).set_retain(1));
        client.publish(MQTT::Publish(prefix + "/"+chipID + "/RELE_1_not/status",    "{\"status\":"+String(state0)+"}").set_retain(1));
        //client.publish(prefix + "/vorota1", "0", true);
        chaing = 0;
        break;
      case 4:
        state0 = !state0;
        digitalWrite(RELE1_PIN, state0);
  //       digitalWrite(RELE2_PIN, state0);
   //     digitalWrite(RELE3_PIN, state0);

        chaing = 0;
        break;


    }

  }
  }
*/
