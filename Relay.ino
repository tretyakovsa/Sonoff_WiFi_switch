void relay_init() {
  pinMode(RELE1_PIN, OUTPUT);
  HTTP.on("/sonoff", sonoffActiv);                //
  HTTP.on("/relay", relayActiv);                //


  modulesReg("relay");
}

// Меняет флаг для включения выключения sonoff
void sonoffActiv() {
  chaing = 1;
  HTTP.send(200, "text/plain", "OK");
}

// Меняет флаг для включения выключения sonoff
void relayActiv() {
  //Language = HTTP.arg("relay");
  //Language = HTTP.arg("trigger");

  HTTP.send(200, "text/plain", "OK");
}


void handleRelay(){
  if (chaing) {
  noInterrupts();
  state0=!state0;
  digitalWrite(RELE1_PIN,state0);
  client.publish(chipID + "/RELE_1", String(state0)); // отправляем в топик
  chaing = 0;
  interrupts();
 }
  }
