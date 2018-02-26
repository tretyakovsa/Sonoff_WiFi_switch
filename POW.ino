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
