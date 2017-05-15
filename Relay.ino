void relay_init() {
  pinMode(RELE1_PIN, OUTPUT);
  sCmd.addCommand("relayon",     relayOn);
  sCmd.addCommand("relayoff",    relayOff);
  sCmd.addCommand("relaynot",    relayNot);
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
