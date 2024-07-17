#include <queue>

static struct Sensor_Buffer {
    uint8_t time;                       // timestamp
    uint8_t idx;                        // index, changes to sensor label when full
    int16_t data[255];
} __attribute__((packed));

std::queue<Sensor_Buffer*> sdQ;
std::queue<Sensor_Buffer*> readQ;
Sensor_Buffer* bufID[7][3];

static void build_buff () {
    uint8_t i, j;
    Sensor_Buffer sbuf[32];
    for (i = 0; i < 32; i++){
        sbuf[i].idx = 0;
        readQ.push(&sbuf);
    }
    for (i = 0; i < 7; i++){
        for (j = 0; j < 3; j++){
            bufID[i][j] = readQ.pop();
        }
    }
}
