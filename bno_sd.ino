// To improve SDA-to-SCL setup time during clock-stretching cycles, try adding a 2.7K preset_pinullup to SDA and a 4.7K pullup to SCL (or similar values), and use short I2C wires (a few cm).
// At 10ms rate, BNO08x outputs most reports in one burst, Gyr-Quat-Lac-Mag, however Acc is asynchronous and a few percent faster. Situation may vary with SENSOR_US and maximum sensor rates.

// To Do: - check to make sure sensors are sampling properly all together (check times between report deliveries + delays when only using 1 sensor)
//        - write code to interpret data quickly

#include <Wire.h>

#define BNOs        7           // number of BNO08x breakouts connected via TCA9548 mux
#define RESET_PIN   4           // BNO reset pin
#define DEBUG       0

// sampling time (us) 10000L : 100Hz, 20000L : 50Hz
#define GYR_Ts      10000L      
#define ACC_Ts      10000L      
#define MAG_Ts      25000L  

#define MUX_ADDR    0x70        // I2C address of TCA9548 multiplexer
#define BNO_ADDR    0x4A        // I2C address of BNO085 sensor (0x4A if SA0=0, 0x4B if SA0=1)
#define I2C_CLOCK   400000L     // I2C clock rate
#define SERIAL_BAUD 230400L     // serial port baud rate

#define ACC_REPORT  0x01        // accel report (6.5.9)
#define GYR_REPORT  0x02        // gyr report (6.5.13)
#define MAG_REPORT  0x03        // magneto report (6.5.16)
#define TIME_REPORT 0xFB        // time report (7.2.1)


void setup() {
  Serial.begin(SERIAL_BAUD);
  pinMode(RESET_PIN, OUTPUT);
  reset();
}

void loop() {
  for (uint8_t id = 0; id < BNOs; id++) {  // check for reports from all BNOs
    if (id == 2){id = 7;}
    check_report(id);
    if (id == 7){id = 2;}
  }
}
