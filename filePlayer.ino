void playFile() {

    uint16_t block_id, block_last_byte_bits, block_len_pause, block_len_pilot, block_pilot_len, block_pulses_data[2], block_pulses_sync[2], block_pulse_pilot;
    uint32_t block_len_data;

  #if defined(ARDUINO_BLUEPILL_F103CB)
    Serial.print("\r\nPlaying: ");
    Serial.print(fileName);
    Serial.print("\r\n\r\n");
  #endif

  initBuffer();

  for(uint8_t i=0;i<10;i++) {
    file.read();
  }

  #if defined(ARDUINO_BLUEPILL_F103CB)
    Timer->setOverflow(PAUSEMS);
    Timer->refresh();
    Timer->resume();
  #else
    OCR1A = PAUSEMS;
    TCNT1 = 0;
    SET(TIMSK1, OCIE1A);
  #endif

  for(;;) {
    block_id = file.read();
    switch(block_id) {
      case ID10:
        block_len_pause = readWord();
        block_len_data = readWord();
        putByte(TONE);
        if(file.peek()>=128) {
          putWord(PILOTDATALENGTH);
        } else {
          putWord(PILOTHEADERLENGTH);
        }
        putWord(PILOTPULSE);
        putByte(PULSES);
        putByte(2);
        putWord(SYNC1PULSE);
        putWord(SYNC2PULSE);
        putByte(DATA);
        put24Bits(block_len_data*16);
        putWord(ZEROPULSE);
        putWord(ONEPULSE);
        putByte(0);
        for(block_len_data;block_len_data>0;block_len_data--) {
          putByte(file.read());
        }
        if(block_len_pause) {
          putByte(PAUSE);
          putWord(block_len_pause);
        }
        break;
      case ID11:
        block_pulse_pilot = readPulse();
        block_pulses_sync[0] = readPulse();
        block_pulses_sync[1] = readPulse();
        block_pulses_data[0] = readPulse();
        block_pulses_data[1] = readPulse();
        block_len_pilot = readWord();
        block_last_byte_bits = file.read();
        block_len_pause = readWord();
        block_len_data = read24Bits();
        putByte(TONE);
        putWord(block_len_pilot);
        putWord(block_pulse_pilot);
        putByte(PULSES);
        putByte(2);
        putWord(block_pulses_sync[0]);
        putWord(block_pulses_sync[1]);
        putByte(DATA);
        put24Bits(block_len_data*16);
        putWord(block_pulses_data[0]);
        putWord(block_pulses_data[1]);
        putByte((8-block_last_byte_bits)*2);
        for(block_len_data;block_len_data>0;block_len_data--) {
          putByte(file.read());
        }
        if(block_len_pause) {
          putByte(PAUSE);
          putWord(block_len_pause);
        }
        break;
      case ID12:
        block_pulse_pilot = readPulse();
        block_len_pilot = readWord();
        putByte(TONE);
        putWord(block_len_pilot);
        putWord(block_pulse_pilot);
        break;
      case ID13:
        block_len_data = file.read();
        putByte(PULSES);
        putByte(block_len_data);
        for(block_len_data;block_len_data>0;block_len_data--) {
          putWord(readPulse());
        }
        break;
      case ID14:
        block_pulses_data[0] = readPulse();
        block_pulses_data[1] = readPulse();
        block_last_byte_bits = file.read();
        block_len_pause = readWord();
        block_len_data = read24Bits();
        putByte(DATA);
        put24Bits(block_len_data*16);
        putWord(block_pulses_data[0]);
        putWord(block_pulses_data[1]);
        putByte((8-block_last_byte_bits)*2);
        for(block_len_data;block_len_data>0;block_len_data--) {
          putByte(file.read());
        }
        if(block_len_pause) {
          putByte(PAUSE);
          putWord(block_len_pause);
        }
        break;
      case ID15:
        block_pulses_data[0] = readPulse();
        block_len_pause = readWord();
        block_last_byte_bits = file.read();
        if(!block_last_byte_bits) {
          block_last_byte_bits = 8;
        }
        block_len_data = read24Bits();
        putByte(SAMPLE);
        put24Bits(block_len_data*8);
        putWord(block_pulses_data[0]);
        putByte(8-block_last_byte_bits);
        for(block_len_data;block_len_data>0;block_len_data--) {
          putByte(file.read());
        }
        if(block_len_pause) {
          putByte(PAUSE);
          putWord(block_len_pause);
        }
        break;
      case ID20:
        block_len_pause = readWord();
        if(block_len_pause) {
          putByte(PAUSE);
          putWord(block_len_pause);
        } else {
          putByte(STOP);
          for(;;);
        }
        break;
      case ID21:
        block_len_data = file.read();
        for(block_len_data;block_len_data>0;block_len_data--) {
          file.read();
        }
        break;
      case ID22:
        break;
      case ID30:
        block_len_data = file.read();
        for(block_len_data;block_len_data>0;block_len_data--) {
          file.read();
        }
        break;
      case ID5A:
        for(uint8_t i=0;i<9;i++) {    // skip the next nine bytes of the file
          file.read();
        }
        break;
      case EOF:
        putByte(STOP);
        for(;;);
      default:
        #if defined(ARDUINO_BLUEPILL_F103CB)
           Serial.print("Unknown block ID: ");
           Serial.print(block_id, HEX);
           Serial.print("\r\n\r\n");
        #endif       
        putByte(STOP);
        for(;;);
               
    }
  }

}
