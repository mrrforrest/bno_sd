// To Do: - check to make sure sensors are sampling properly all together (check times between report deliveries + delays when only using 1 sensor)
//        - write code to interpret data quickly

#include <Wire.h>
#include <queue>
#include <SPI.h>
#include <SdFat.h>

#define BNOs        7           // number of BNO08x breakouts connected via TCA9548 mux
#define RESET_PIN   4           // BNO reset pin

#define GYR_Ts      10000L      // sampling time (us) 10000L : 100Hz, 20000L : 50Hz    
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

#define FILE_BASE_NAME "Data"

static struct Sensor_Buffer {
    uint8_t time;                       // timestamp
    uint8_t idx;                        // index, changes to sensor label when full
    int16_t data[255];
} __attribute__((packed));

const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
const uint8_t chipSelect = SS; // needed?

uint8_t data[3];                // 2D array of booleans, 1 if data has been received for ACC/GYR/MAG and BNO index

SdFat sd;
SdFile file;
Sensor_Buffer* bufID[BNOs][3];

std::queue<Sensor_Buffer*> sdQ;
std::queue<Sensor_Buffer*> readQ;

void setup() {
  Serial.begin(SERIAL_BAUD);
  pinMode(RESET_PIN, OUTPUT);
  reset();
  build_buff();
  file_setup();
  xTaskCreatePinnedToCore( sensor_read, "Task1", 10000, NULL, 0, &Task1, 0); // needs tuning for word allocation
}

static void sensor_read () {
    while (1) {
        for (uint8_t id = 0; id < BNOs; id++) {  // check for reports from all BNOs
            if (id == 2){id = 7;}
            check_report(id);
            if (id == 7){id = 2;}
        }
    } 
}

void loop () {
    if (sdQ::size() >= 16) {
        logData();
    }
}
