// save data to sd

#include <SPI.h>
#include <SdFat.h>

#define FILE_BASE_NAME "Data"
const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
const uint8_t chipSelect = SS; // needed?

SdFat sd;
SdFile file;

static void logData () {
    Sensor_Buffer* ptr;
    for (uint8_t i = 0; i < 16; i++){
        ptr = sdQ.pop();
        file.write(*ptr);
        readQ.push(ptr);
    }
    file.sync();
}

static void file_setup (){
    char fileName[13] = FILE_BASE_NAME "00.bin";
    sd.begin(chipSelect, SPI_HALF_SPEED); // SPI_HALF_SPEED->avoid errors  SPI_FULL_SPEED->performance
    while (sd.exists(fileName)) { // Find an unused file name.
        if (fileName[BASE_NAME_SIZE + 1] != '9') {fileName[BASE_NAME_SIZE + 1]++;} 
        else {fileName[BASE_NAME_SIZE + 1] = '0'; fileName[BASE_NAME_SIZE]++;}
    }
    file.open(fileName, O_CREAT | O_WRITE | O_EXCL);
}
