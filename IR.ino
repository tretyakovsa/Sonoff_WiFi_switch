#ifdef irTransmitterM
// ----------------------Передатчик ИK
void irTransmitter() {
  String moduleName = "irTransmitter";
  //  Serial.println(moduleName);
  byte pin = readArgsInt();
  pin =  pinTest(pin);
  irSender = new IRsend(pin);  // Create a new IRrecv object. Change to what ever pin you need etc.
  irSender->begin();
  sCmd.addCommand("irsend", handleIrTransmit);
  commandsReg(irsendS);
  modulesReg(moduleName);
}

// команда irsend
void handleIrTransmit() {
  uint8_t ir_type = readArgsInt(); // Тип сигнала
  String code_str = readArgsString(); // Код
  uint16_t repeat = readArgsInt();    // Повтор
  uint8_t bits = readArgsInt();       // Длинна кода
  uint32_t  code = strtol(("0x" + code_str).c_str(), NULL, 0);
  //irSender->sendNEC(tmp, 32);

  switch (ir_type) {
#if SEND_RC5
    case RC5:  // 1
      if (bits == 0)
        bits = RC5_BITS;
      irSender->sendRC5(code, bits, repeat);
      break;
#endif
#if SEND_RC6
    case RC6:  // 2
      if (bits == 0)
        bits = RC6_MODE0_BITS;
      irSender->sendRC6(code, bits, repeat);
      break;
#endif
#if SEND_NEC
    case NEC:  // 3
      if (bits == 0)
        bits = NEC_BITS;
      irSender->sendNEC(code, bits, repeat);
      break;
#endif
#if SEND_SONY
    case SONY:  // 4
      if (bits == 0)
        bits = SONY_12_BITS;
      repeat = std::max(repeat, (uint16_t) SONY_MIN_REPEAT);
      irSender->sendSony(code, bits, repeat);
      break;
#endif
#if SEND_PANASONIC
    case PANASONIC:  // 5
      if (bits == 0)
        bits = PANASONIC_BITS;
      irSender->sendPanasonic64(code, bits, repeat);
      break;
#endif
#if SEND_JVC
    case JVC:  // 6
      if (bits == 0)
        bits = JVC_BITS;
      irSender->sendJVC(code, bits, repeat);
      break;
#endif
#if SEND_SAMSUNG
    case SAMSUNG:  // 7
      if (bits == 0)
        bits = SAMSUNG_BITS;
      irSender->sendSAMSUNG(code, bits, repeat);
      break;
#endif
#if SEND_WHYNTER
    case WHYNTER:  // 8
      if (bits == 0)
        bits = WHYNTER_BITS;
      irSender->sendWhynter(code, bits, repeat);
      break;
#endif
#if SEND_AIWA_RC_T501
    case AIWA_RC_T501:  // 9
      if (bits == 0)
        bits = AIWA_RC_T501_BITS;
      repeat = std::max(repeat, (uint16_t) AIWA_RC_T501_MIN_REPEAT);
      irSender->sendAiwaRCT501(code, bits, repeat);
      break;
#endif
#if SEND_LG
    case LG:  // 10
      if (bits == 0)
        bits = LG_BITS;
      irSender->sendLG(code, bits, repeat);
      break;
#endif
#if SEND_MITSUBISHI
    case MITSUBISHI:  // 12
      if (bits == 0)
        bits = MITSUBISHI_BITS;
      repeat = std::max(repeat, (uint16_t) MITSUBISHI_MIN_REPEAT);
      irSender->sendMitsubishi(code, bits, repeat);
      break;
#endif
#if SEND_DISH
    case DISH:  // 13
      if (bits == 0)
        bits = DISH_BITS;
      repeat = std::max(repeat, (uint16_t) DISH_MIN_REPEAT);
      irSender->sendDISH(code, bits, repeat);
      break;
#endif
#if SEND_SHARP
    case SHARP:  // 14
      if (bits == 0)
        bits = SHARP_BITS;
      irSender->sendSharpRaw(code, bits, repeat);
      break;
#endif
#if SEND_COOLIX
    case COOLIX:  // 15
      if (bits == 0)
        bits = COOLIX_BITS;
      irSender->sendCOOLIX(code, bits, repeat);
      break;
#endif
    case DAIKIN:  // 16
    case KELVINATOR:  // 18
    case MITSUBISHI_AC:  // 20
    case GREE:  // 24
    case ARGO:  // 27
    case TROTEC:  // 28
    case TOSHIBA_AC:  // 32
    case FUJITSU_AC:  // 33
    case HAIER_AC:  // 38
    case HITACHI_AC:  // 40
    case HITACHI_AC1:  // 41
    case HITACHI_AC2:  // 42
      parseStringAndSendAirCon(ir_type, code_str);
      break;
#if SEND_DENON
    case DENON:  // 17
      if (bits == 0)
        bits = DENON_BITS;
      irSender->sendDenon(code, bits, repeat);
      break;
#endif
#if SEND_SHERWOOD
    case SHERWOOD:  // 19
      if (bits == 0)
        bits = SHERWOOD_BITS;
      repeat = std::max(repeat, (uint16_t) SHERWOOD_MIN_REPEAT);
      irSender->sendSherwood(code, bits, repeat);
      break;
#endif
#if SEND_RCMM
    case RCMM:  // 21
      if (bits == 0)
        bits = RCMM_BITS;
      irSender->sendRCMM(code, bits, repeat);
      break;
#endif
#if SEND_SANYO
    case SANYO_LC7461:  // 22
      if (bits == 0)
        bits = SANYO_LC7461_BITS;
      irSender->sendSanyoLC7461(code, bits, repeat);
      break;
#endif
#if SEND_RC5
    case RC5X:  // 23
      if (bits == 0)
        bits = RC5X_BITS;
      irSender->sendRC5(code, bits, repeat);
      break;
#endif
#if SEND_PRONTO
    case PRONTO:  // 25
      //      parseStringAndSendPronto(code_str, repeat);
      break;
#endif
#if SEND_NIKAI
    case NIKAI:  // 29
      if (bits == 0)
        bits = NIKAI_BITS;
      irSender->sendNikai(code, bits, repeat);
      break;
#endif
#if SEND_RAW
    case RAW:  // 30
      //      parseStringAndSendRaw(code_str);
      break;
#endif
#if SEND_GLOBALCACHE
    case GLOBALCACHE:  // 31
      //      parseStringAndSendGC(code_str);
      break;
#endif
#if SEND_MIDEA
    case MIDEA:  // 34
      if (bits == 0)
        bits = MIDEA_BITS;
      irSender->sendMidea(code, bits, repeat);
      break;
#endif
#if SEND_MAGIQUEST
    case MAGIQUEST:  // 35
      if (bits == 0)
        bits = MAGIQUEST_BITS;
      irSender->sendMagiQuest(code, bits, repeat);
      break;
#endif
#if SEND_LASERTAG
    case LASERTAG:  // 36
      if (bits == 0)
        bits = LASERTAG_BITS;
      irSender->sendLasertag(code, bits, repeat);
      break;
#endif
#if SEND_CARRIER_AC
    case CARRIER_AC:  // 37
      if (bits == 0)
        bits = CARRIER_AC_BITS;
      irSender->sendCarrierAC(code, bits, repeat);
      break;
#endif
#if SEND_MITSUBISHI2
    case MITSUBISHI2:  // 39
      if (bits == 0)
        bits = MITSUBISHI_BITS;
      repeat = std::max(repeat, (uint16_t) MITSUBISHI_MIN_REPEAT);
      irSender->sendMitsubishi2(code, bits, repeat);
      break;
#endif
#if SEND_GICABLE
    case GICABLE:  // 43
      if (bits == 0)
        bits = GICABLE_BITS;
      repeat = std::max(repeat, (uint16_t) GICABLE_BITS);
      irSender->sendGICable(code, bits, repeat);
      break;
#endif

  }
}

// Parse an Air Conditioner A/C Hex String/code and send it.
// Args:
//   irType: Nr. of the protocol we need to send.
//   str: A hexadecimal string containing the state to be sent.
void parseStringAndSendAirCon(const uint16_t irType, const String str) {
  uint8_t strOffset = 0;
  uint8_t state[STATE_SIZE_MAX] = {0};  // All array elements are set to 0.
  uint16_t stateSize = 0;

  if (str.startsWith("0x") || str.startsWith("0X"))
    strOffset = 2;
  // Calculate how many hexadecimal characters there are.
  uint16_t inputLength = str.length() - strOffset;
  if (inputLength == 0) {
    //    debug("Zero length AirCon code encountered. Ignored.");
    return;  // No input. Abort.
  }

  switch (irType) {  // Get the correct state size for the protocol.
    case KELVINATOR:
      stateSize = KELVINATOR_STATE_LENGTH;
      break;
    case TOSHIBA_AC:
      stateSize = TOSHIBA_AC_STATE_LENGTH;
      break;
    case DAIKIN:
      stateSize = DAIKIN_COMMAND_LENGTH;
      break;
    case MITSUBISHI_AC:
      stateSize = MITSUBISHI_AC_STATE_LENGTH;
      break;
    case TROTEC:
      stateSize = TROTEC_COMMAND_LENGTH;
      break;
    case ARGO:
      stateSize = ARGO_COMMAND_LENGTH;
      break;
    case GREE:
      stateSize = GREE_STATE_LENGTH;
      break;
    case FUJITSU_AC:
      // Fujitsu has four distinct & different size states, so make a best guess
      // which one we are being presented with based on the number of
      // hexadecimal digits provided. i.e. Zero-pad if you need to to get
      // the correct length/byte size.
      stateSize = inputLength / 2;  // Every two hex chars is a byte.
      // Use at least the minimum size.
      stateSize = std::max(stateSize,
                           (uint16_t) (FUJITSU_AC_STATE_LENGTH_SHORT - 1));
      // If we think it isn't a "short" message.
      if (stateSize > FUJITSU_AC_STATE_LENGTH_SHORT)
        // Then it has to be at least the smaller version of the "normal" size.
        stateSize = std::max(stateSize,
                             (uint16_t) (FUJITSU_AC_STATE_LENGTH - 1));
      // Lastly, it should never exceed the maximum "normal" size.
      stateSize = std::min(stateSize, (uint16_t) FUJITSU_AC_STATE_LENGTH);
      break;
    case HAIER_AC:
      stateSize = HAIER_AC_STATE_LENGTH;
      break;
    case HITACHI_AC:
      stateSize = HITACHI_AC_STATE_LENGTH;
      break;
    case HITACHI_AC1:
      stateSize = HITACHI_AC1_STATE_LENGTH;
      break;
    case HITACHI_AC2:
      stateSize = HITACHI_AC2_STATE_LENGTH;
      break;
    default:  // Not a protocol we expected. Abort.
      //      debug("Unexpected AirCon protocol detected. Ignoring.");
      return;
  }
  if (inputLength > stateSize * 2) {
    //    debug("AirCon code to large for the given protocol.");
    return;
  }

  // Ptr to the least significant byte of the resulting state for this protocol.
  uint8_t *statePtr = &state[stateSize - 1];

  // Convert the string into a state array of the correct length.
  for (uint16_t i = 0; i < inputLength; i++) {
    // Grab the next least sigificant hexadecimal digit from the string.
    uint8_t c = tolower(str[inputLength + strOffset - i - 1]);
    if (isxdigit(c)) {
      if (isdigit(c))
        c -= '0';
      else
        c = c - 'a' + 10;
    } else {
      //      debug("Aborting! Non-hexadecimal char found in AirCon state: " + str);
      return;
    }
    if (i % 2 == 1) {  // Odd: Upper half of the byte.
      *statePtr += (c << 4);
      statePtr--;  // Advance up to the next least significant byte of state.
    } else {  // Even: Lower half of the byte.
      *statePtr = c;
    }
  }

  // Make the appropriate call for the protocol type.
  switch (irType) {
#if SEND_KELVINATOR
    case KELVINATOR:
      irSender->sendKelvinator(reinterpret_cast<uint8_t *>(state));
      break;
#endif
#if SEND_TOSHIBA_AC
    case TOSHIBA_AC:
      irSender->sendToshibaAC(reinterpret_cast<uint8_t *>(state));
      break;
#endif
#if SEND_DAIKIN
    case DAIKIN:
      irSender->sendDaikin(reinterpret_cast<uint8_t *>(state));
      break;
#endif
#if MITSUBISHI_AC
    case MITSUBISHI_AC:
      irSender->sendMitsubishiAC(reinterpret_cast<uint8_t *>(state));
      break;
#endif
#if SEND_TROTEC
    case TROTEC:
      irSender->sendTrotec(reinterpret_cast<uint8_t *>(state));
      break;
#endif
#if SEND_ARGO
    case ARGO:
      irSender->sendArgo(reinterpret_cast<uint8_t *>(state));
      break;
#endif
#if SEND_GREE
    case GREE:
      irSender->sendGree(reinterpret_cast<uint8_t *>(state));
      break;
#endif
#if SEND_FUJITSU_AC
    case FUJITSU_AC:
      irSender->sendFujitsuAC(reinterpret_cast<uint8_t *>(state), stateSize);
      break;
#endif
#if SEND_HAIER_AC
    case HAIER_AC:
      irSender->sendHaierAC(reinterpret_cast<uint8_t *>(state));
      break;
#endif
#if SEND_HITACHI_AC
    case HITACHI_AC:
      irSender->sendHitachiAC(reinterpret_cast<uint8_t *>(state));
      break;
#endif
#if SEND_HITACHI_AC1
    case HITACHI_AC1:
      irSender->sendHitachiAC1(reinterpret_cast<uint8_t *>(state));
      break;
#endif
#if SEND_HITACHI_AC2
    case HITACHI_AC2:
      irSender->sendHitachiAC2(reinterpret_cast<uint8_t *>(state));
      break;
#endif
  }
}
#endif
