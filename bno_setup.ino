uint8_t data[3];                // 2D array of booleans, 1 if data has been received for ACC/GYR/MAG and BNO index

static void request_reports (uint8_t id) {
  contact(id);

  static const uint8_t cmd_gyr[]  = {21, 0, 2, 0, 0xFD, GYR_REPORT,  0, 0, 0, (GYR_Ts>>0)&255, (GYR_Ts>>8)&255, (GYR_Ts>>16)&255, (GYR_Ts>>24)&255, 0, 0, 0, 0, 0, 0, 0, 0};
  Wire.beginTransmission(BNO_ADDR);  Wire.write(cmd_gyr, sizeof(cmd_gyr));  Wire.endTransmission();

  static const uint8_t cmd_acc[] = {21, 0, 2, 0, 0xFD, ACC_REPORT, 0, 0, 0, (ACC_Ts>>0)&255, (ACC_Ts>>8)&255, (ACC_Ts>>16)&255, (ACC_Ts>>24)&255, 0, 0, 0, 0, 0, 0, 0, 0};
  Wire.beginTransmission(BNO_ADDR);  Wire.write(cmd_acc, sizeof(cmd_acc));  Wire.endTransmission();

  static const uint8_t cmd_mag[] = {21, 0, 2, 0, 0xFD, MAG_REPORT, 0, 0, 0, (MAG_Ts>>0)&255, (MAG_Ts>>8)&255, (MAG_Ts>>16)&255, (MAG_Ts>>24)&255, 0, 0, 0, 0, 0, 0, 0, 0};
  Wire.beginTransmission(BNO_ADDR);  Wire.write(cmd_mag, sizeof(cmd_mag));  Wire.endTransmission();
}

static void hard_reset () {         // reset over RST pin, restart I2C
  digitalWrite(RESET_PIN, LOW);     // reset
  delay(100);
  digitalWrite(RESET_PIN, HIGH);
  delay(300);
  Wire.begin();                     // initialize I2C
  Wire.setClock(I2C_CLOCK);
  for (uint8_t i = 0; i < 3; i++){  // reset all data
    data[i] = 0;
  }
}

static uint8_t check_data(uint8_t id) {
  uint8_t n = 0, print = 0;
  for (uint8_t i = 0; i < 255; i++) {
    check_report(id);
    if (!missing_data(id, i % 64)) { return 0; }
    delay(20);
  }
  return 1;
}

static uint8_t missing_data(uint8_t id, uint8_t print){  // check sensor for data, return 0 if data
  uint8_t i, val, count = 0;
  for (i = 0; i < 3; i++){
    val = data[i] >> id;
    if(!print){ Serial.print(val); }
    count += val;
  }
  if(!print){ Serial.println(); }
  return (data[0] >> id) + (data[1] >> id) + (data[2] >> id) - 3;
}

static void reset () {          // reset sensors and check for data
  hard_reset();                 // reset BNOS, restart I2C

  for (uint8_t id = 0; id < BNOs; id++) {
    if (id == 2){id = 7;}
    Serial.print(F("Setup IMU ")); Serial.println(id);

    request_reports(id);

    if(check_data(id)){         // reset again if not working
      Serial.println("Retrying reset..."); reset(); return;
    }
    if (id == 7){id = 2;}
  }
  Serial.println("Reset complete!");
}
