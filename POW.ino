// Импульс 1 Гц на выводе CF1 означает 15 мА или 0,5 В RMS в зависимости от значения в выводе SEL
void ICACHE_RAM_ATTR hlw8012_cf1_interrupt() {
  hlw8012.cf1_interrupt();
}
// Импульс 1 Гц на выводе CF составляет около 12 Вт RMS
void ICACHE_RAM_ATTR hlw8012_cf_interrupt() {
  hlw8012.cf_interrupt();
}

// ---------------------- Измеритель мощьности POW
void initHLW8012() {
  String temp = readArgsString();
  byte pinCF;
  if (temp == "") pinCF = pinTest(14);
  else pinCF = pinTest(temp.toInt());
  temp = readArgsString();
  byte pinCF1;
  if (temp == "") pinCF1 = pinTest(13);
  else pinCF1 = pinTest(temp.toInt());
  temp = readArgsString();
  byte pinSEL;
  if (temp == "") pinSEL = pinTest(5);
  else pinSEL = pinTest(temp.toInt());
  hlw8012.begin(pinCF, pinCF1, pinSEL, HIGH, true);
  hlw8012.setResistors(CURRENT_RESISTOR, VOLTAGE_RESISTOR_UPSTREAM, VOLTAGE_RESISTOR_DOWNSTREAM);
  modulesReg("POW");
  attachInterrupt(pinCF1, hlw8012_cf1_interrupt, CHANGE);
  attachInterrupt(pinCF, hlw8012_cf_interrupt, CHANGE);
  calibrate();
}

void calibrate() {
  // Let some time to register values
  unsigned long timeout = millis();
  while ((millis() - timeout) < 10000) {
    delay(1);
  }
  // Calibrate using a 60W bulb (pure resistive) on a 230V line
  hlw8012.expectedActivePower(60.0);
  hlw8012.expectedVoltage(220.0);
  hlw8012.expectedCurrent(60.0 / 220.0);
  sendOptions("current", hlw8012.getCurrentMultiplier());
  sendOptions("voltage", hlw8012.getVoltageMultiplier());
  sendOptions("power", hlw8012.getPowerMultiplier());
}

void initFurnace(){
  uint8_t pin1 = readArgsInt(); // первый аргумент pin1
  uint8_t pin2 = readArgsInt(); // первый аргумент pin2
  String num = readArgsString(); // третьий аргумент прификс конфорки 0 1 2
  sendStatus(stateFurnaceS + num, LOW);
  sendOptions(furnacePin1S + num, pin1);
  sendOptions(furnacePin2S + num, pin2);
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  digitalWrite(pin1,LOW);
  digitalWrite(pin2,LOW);
  sCmd.addCommand("furnace", furnaceGo);
  commandsReg("furnace");
  modulesReg("furnace");
  }

void furnaceGo(){
  // команда furnace 0 on 0-1-2-3
  String num = readArgsString();
  String com = readArgsString();
  String state = readArgsString();
  uint8_t pin1 = getOptionsInt(furnacePin1S + num);
  uint8_t pin2 = getOptionsInt(furnacePin2S + num);
  Serial.println("furnace");
  if (state != "") {
          }
      if (com == "on") {
  digitalWrite(pin1,HIGH);
  digitalWrite(pin2,HIGH);
  sendStatus(stateFurnaceS + num, HIGH);
      }
        if (com == "off") {
  digitalWrite(pin1,LOW);
  digitalWrite(pin2,LOW);
   sendStatus(stateFurnaceS + num, LOW);
      }
  }


