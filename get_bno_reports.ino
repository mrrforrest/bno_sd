static void check_report (uint8_t id) {
    int16_t length = read_header(id);      // length of report
    uint8_t buf[9];                        // report buffer
    uint8_t report_name;                   // report type

    while (length) {                       // while more reports in cargo
        ensure_read_available(length);
        report_name = Wire.read();               
        length--;
        if(length >= 9) {                  // acc, mag, gyr length 9 on channel 3, time ignored if no later report
        if (report_name <= 3 && report_name >= 1){
            read_report(9, length, buf);
            length -= 9;
            add_report(buf, id, report_name);
            data[report_name - 1] |= 1 << id;
            continue;
        }
        if (report_name == TIME_REPORT) {
            dump_bytes(4, length);
            length -= 4;
            continue;
        }
    }
    dump_bytes(length, length); // discard remaining cargo
  }
  return;
}

static void add_report(int16_t buff, uint8_t id, uint8_t type){
    type--;
    int16_t* next_data = bufID[id][type]->data + bufID[id][type]->idx;  // convert index to ptr (keeps in 512B)
    *next_data++ = (int16_t)buff[7];
    *next_data++ = (int16_t)buff[3];
    *next_data = (int16_t)buff[5];
    idx += 3;

    if(bufID[id][type]->idx == 255){
        bufID[id][type]->idx = (id << 2) | type;      // XXXBBBSS B:bno S:sensor type -> labels data for file 
        sdQ.push(bufID[id][type]);

        bufID[id][type] = readQ.pop();
        bufID[id][type]->idx = 0;
        bufID[id][type]->time = millis() % 255;       // timestamp
    }
}
