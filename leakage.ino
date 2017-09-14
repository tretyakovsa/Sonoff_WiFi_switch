void initLeakage() {
  int pinMotor1A = readArgsInt();
  int pinMotor2A = readArgsInt();
  int pinMotor3A = readArgsInt();
  int pinMotor4A = readArgsInt();
  int pinMotor1B = readArgsInt();
  int pinMotor2B = readArgsInt();
  int pinMotor3B = readArgsInt();
  int pinMotor4B = readArgsInt();
  configLive = jsonWrite(configLive, "pinMotor1A", pinMotor1A);
  configLive = jsonWrite(configLive, "pinMotor2A", pinMotor2A);
  configLive = jsonWrite(configLive, "pinMotor3A", pinMotor3A);
  configLive = jsonWrite(configLive, "pinMotor4A", pinMotor4A);
  configLive = jsonWrite(configLive, "pinMotor1B", pinMotor1B);
  configLive = jsonWrite(configLive, "pinMotor2B", pinMotor2B);
  configLive = jsonWrite(configLive, "pinMotor3B", pinMotor3B);
  configLive = jsonWrite(configLive, "pinMotor4B", pinMotor4B);
  configJson = jsonWrite(configJson, "stateLeakageA", 1);
  configJson = jsonWrite(configJson, "stateLeakageB", 1);
  initMotor(pinMotor1A, pinMotor2A, pinMotor3A, pinMotor4A);
  initMotor(pinMotor1B, pinMotor2B, pinMotor3B, pinMotor4B);
  //sCmd.addCommand("leakageopen",     leakageOpen);
  //sCmd.addCommand("leakageclose",    leakageClose);
  //sCmd.addCommand("leakagenot",    leakageNot);
  //sCmd.addCommand("leakagestop",    leakageStop);
  modulesReg("leakage");

}

void initMotor(int pinMotor1, int pinMotor2, int pinMotor3, int pinMotor4) {
  pinMode(pinMotor1, OUTPUT);
  pinMode(pinMotor2, OUTPUT);
  pinMode(pinMotor3, OUTPUT);
  pinMode(pinMotor4, OUTPUT);
}

void steperTurn(String motor, int steps, boolean ) {

}

void leakageMotor() {
  configLive = jsonWrite(configLive, "A", 250);
  configLive = jsonWrite(configLive, "B", 250);
  motion.attach(10, motionOff);
  command = jsonRead(configJson, "Command") + "on";

}
void leakageOff() {
  motion.detach();
  command = jsonRead(configJson, "Command") + "off";

}

void stepMotor(int thisStep, String motor) {
  switch (thisStep) {
    case 0:  // 1010
      digitalWrite(jsonReadtoInt(configLive, "pinMotor1" + motor), HIGH);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor2" + motor), LOW);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor3" + motor), HIGH);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor4" + motor), LOW);
      break;
    case 1:  // 0110
      digitalWrite(jsonReadtoInt(configLive, "pinMotor1" + motor), LOW);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor2" + motor), HIGH);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor3" + motor), HIGH);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor4" + motor), LOW);
      break;
    case 2:  //0101
      digitalWrite(jsonReadtoInt(configLive, "pinMotor1" + motor), LOW);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor2" + motor), HIGH);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor3" + motor), LOW);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor4" + motor), HIGH);
      break;
    case 3:  //1001
      digitalWrite(jsonReadtoInt(configLive, "pinMotor1" + motor), HIGH);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor2" + motor), LOW);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor3" + motor), LOW);
      digitalWrite(jsonReadtoInt(configLive, "pinMotor4" + motor), HIGH);
      break;
  }
}
