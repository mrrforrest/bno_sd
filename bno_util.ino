#define BNOs        7           // number of BNO08x breakouts connected via TCA9548 mux
#define RESET_PIN   4           // BNO reset pin

// sampling time (us) 10000L : 100Hz, 20000L : 50Hz
#define GYR_Ts      10000L      
#define ACC_Ts      10000L      
#define MAG_Ts      30000L  

#define MUX_ADDR    0x70        // I2C address of TCA9548 multiplexer
#define BNO_ADDR    0x4A        // I2C address of BNO085 sensor (0x4A if SA0=0, 0x4B if SA0=1)
#define I2C_CLOCK   400000L     // I2C clock rate
#define SERIAL_BAUD 230400L     // serial port baud rate

#define ACC_REPORT  0x01        // accel report (6.5.9)
#define GYR_REPORT  0x02        // gyr report (6.5.13)
#define MAG_REPORT  0x03        // magneto report (6.5.16)
#define TIME_REPORT 0xFB        // time report (7.2.1)

uint8_t data[3];                // 2D array of booleans, 1 if data has been received for ACC/GYR/MAG and BNO index

static char output[300];        // ensure this output buffer is big enough for your output string!
char *next_char;                // pointer to set output 

static void contact (uint8_t id) {  // select BNO
  Wire.beginTransmission(MUX_ADDR); 
  Wire.write(1 << id);
  Wire.endTransmission();
  return;
}

static void dump_bytes (uint8_t b, int16_t length) { // ignore b bytes
    uint8_t i = 0;
    uint8_t avl = ensure_read_available(length);
    while (b > avl) { // another re-read is needed
        for (i; i < avl; i++) { Wire.read(); }
        length -= avl;
        avl += ensure_read_available(length);
    }
    for (uint8_t i = 0; i < b; i++) { Wire.read(); }
}

static void read_report (uint8_t b, int16_t length, uint8_t* buf) { // read report of length b into buf
    uint8_t i = 0;
    uint8_t avl = ensure_read_available(length);
    while (b > avl) { // another re-read is needed
        for (i; i < avl; i++) { buf[i] = Wire.read(); }
        length -= avl;
        avl += ensure_read_available(length);
    }
    for (i; i < b; i++) { buf[i] = Wire.read(); }
}

static void ensure_read_available (int16_t length)  { // ensure a read byte is available
    uint8_t avl = Wire.available();
    if (avl) {return avl;}
    Wire.requestFrom(BNO_ADDR, 4+length); // re-read
    dump_bytes(4, length); // discard SHTP header
    return Wire.available();
}

int16_t static void read_header (uint8_t id) {
    int16_t length;
    uint8_t channel;

    contact(id);
    Wire.requestFrom(BNO_ADDR, 32);       // max requestable amount

    length  = Wire.read();                // length LSB
    length |= (Wire.read() & 0x7F) << 8;  // length MSB (ignore continuation flag)

    if (length <= 0 || length > 1000) {    // if null/bad/degenerate SHTP header
        return 0;
    }

    channel = Wire.read();                // channel number
    Wire.read();                          // sequence number (ignore)
    length -= 4;                          // done reading SHTP Header

    if (channel != 3 || Wire.read() != TIME_REPORT) { 
        dump_bytes(length); 
        return 0;
    }
    dump_bytes (4, length);
    return length;
}
