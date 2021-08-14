void initBuffer() {

  buffer_read = buffer_write = 0;
  for(uint16_t i=0;i<BUFFERSIZE;i++) {
    buffer[i] = 0;
  }  
  
}

uint8_t getByte() {
  
  uint8_t x = buffer[buffer_read];
  
  buffer_read = ((buffer_read+1)&(BUFFERSIZE-1));
    
  return x;

}

uint16_t getWord() {

  return getByte()|getByte()<<8;
  
}

uint32_t get24Bits() {

  #if defined(ARDUINO_BLUEPILL_F103CB)
    return getWord()|getByte()<<16;
  #else
    return getWord()|uint32_t(getByte())<<16;
  #endif
  
}

void putByte(uint8_t x) {

  while(((buffer_write+1)&(BUFFERSIZE-1))==buffer_read);
  buffer[buffer_write] = x;
  buffer_write = ((buffer_write+1)&(BUFFERSIZE-1));
//  Serial.print(buffer_write);
//  Serial.print(" : ");
//  Serial.print(x, HEX);
//  Serial.print("\r\n");
  
}

uint16_t readWord() {

  return file.read()|file.read()<<8;
  
}

uint32_t read24Bits() {

  #if defined(ARDUINO_BLUEPILL_F103CB)
    return readWord()|file.read()<<16;
  #else
    return readWord()|uint32_t(file.read())<<16;
  #endif
  
}

uint16_t readPulse() {

  #if defined(ARDUINO_BLUEPILL_F103CB)
    return (((readWord()*21065)>>8)+8)>>4;
  #else
    return (((readWord()*37449)>>8)+16)>>5;
  #endif

}

void putWord(uint16_t x) {

  putByte(x);
  putByte(x>>8);
  
}

void put24Bits(uint32_t x) {

  putByte(x);
  putWord(x>>8);
  
}
