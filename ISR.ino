#if defined(ARDUINO_BLUEPILL_F103CB)
  void output(void) {

    switch(current_signal) {
      case PULSES:
        HAL_GPIO_TogglePin(EARPORT, EARPIN);
        break;
      case SAMPLE:
        if(current_sample) {
          HAL_GPIO_WritePin(EARPORT, EARPIN, GPIO_PIN_RESET);
        } else {
          HAL_GPIO_WritePin(EARPORT, EARPIN, GPIO_PIN_SET);       
        }
        break;
      case DATA:
        HAL_GPIO_TogglePin(EARPORT, EARPIN);
        break;
      case TONE:
        HAL_GPIO_TogglePin(EARPORT, EARPIN);
        break;
      case PAUSE:
        HAL_GPIO_WritePin(EARPORT, EARPIN, GPIO_PIN_RESET);
        break;
      case STOP:
        HAL_GPIO_WritePin(EARPORT, EARPIN, GPIO_PIN_RESET);
        Timer->pause();
        return;
      default:
        break;
    }
    current_signal = next_signal;
    if(!(pulses-last_byte_adjust)) {
      next_signal = getByte();
      last_byte_adjust = 0;
      switch(next_signal) {
        case PULSES:
          pulses = getByte();
          Serial.print("PULSES: ");
          Serial.print("length ");
          Serial.print(pulses);
          Serial.print("\r\n");  
          break;
        case SAMPLE:
          pulses = get24Bits();
          symbols[0] = getWord();
          last_byte_adjust = getByte();
          Serial.print("SAMPLE: ");
          Serial.print("length ");
          Serial.print(pulses);
          Serial.print(", rate ");
          Serial.print(symbols[0]);
          Serial.print("\r\n");
          break;
        case DATA:
          pulses = get24Bits();
          symbols[0] = getWord();
          symbols[1] = getWord(); 
          last_byte_adjust = getByte();
          Serial.print("DATA: ");
          Serial.print("length ");
          Serial.print(pulses);
          Serial.print(", zero ");
          Serial.print(symbols[0]);
          Serial.print(", one ");
          Serial.print(symbols[1]);
          Serial.print("\r\n");
          break;
        case TONE:
          pulses = getWord();
          symbols[0] = getWord();
          Serial.print("TONE: ");
          Serial.print("length ");
          Serial.print(pulses);
          Serial.print(", pulse ");
          Serial.print(symbols[0]);
          Serial.print("\r\n");
          break;
        case PAUSE:
          pulses = getWord();
          Serial.print("PAUSE: ");
          Serial.print("length ");
          Serial.print(pulses);
          Serial.print("\r\n");
          break;
        case NONE:
          return;
        case STOP:
          Serial.print("STOP");
          Serial.print("\r\n");
        default:
          break;
      }
    }
    switch(next_signal) {
      case PULSES:
        Timer->setOverflow(getWord());        
        break;
      case SAMPLE:
        if(!(pulses&7)) {
          current_byte = getByte();
        } else {
          current_byte <<= 1;
        }
        current_sample = current_byte&128;
        Timer->setOverflow(symbols[0]); 
        break;
      case DATA:
        if(!(pulses&15)) {
          current_byte = getByte();
        } else {
          if(!(pulses&1)) {
            current_byte <<= 1;
          }
        }
        Timer->setOverflow(symbols[bool(current_byte&128)]); 
        break;
      case TONE:
        Timer->setOverflow(symbols[0]); 
        break;
      case PAUSE:
        Timer->setOverflow(PAUSEMS); 
      }
    pulses--;
  
  }
#else
  ISR(TIMER1_COMPA_vect) {

    switch(current_signal) {
      case PULSES:
        TOGGLE(EARPORT, EARPIN);
        break;
      case SAMPLE:
        if(current_sample) {
          CLEAR(EARPORT, EARPIN);
        } else {
          SET(EARPORT, EARPIN);       
        }
        break;
      case DATA:
        TOGGLE(EARPORT, EARPIN);
        break;
      case TONE:
        TOGGLE(EARPORT, EARPIN);
        break;
      case PAUSE:
        CLEAR(EARPORT, EARPIN);
        break;
      case STOP:
        CLEAR(EARPORT, EARPIN);
        CLEAR(TIMSK1, OCIE1A);
        return;
      default:
        break;
    }
    current_signal = next_signal;
    if(!(pulses-last_byte_adjust)) {
      next_signal = getByte();
      last_byte_adjust = 0;
      switch(next_signal) {
        case PULSES:
          pulses = getByte();  
          break;
        case SAMPLE:
          pulses = get24Bits();
          symbols[0] = getWord();
          last_byte_adjust = getByte();
          break;
        case DATA:
          pulses = get24Bits();
          symbols[0] = getWord();
          symbols[1] = getWord(); 
          last_byte_adjust = getByte();
          break;
        case TONE:
          pulses = getWord();
          symbols[0] = getWord();
          break;
        case PAUSE:
          pulses = getWord();
          break;
        case NONE:
          return;
        case STOP:
        default:
          break;
      }
    }
    switch(next_signal) {
      case PULSES:
        OCR1A = getWord();       
        break;
      case SAMPLE:
        if(!(pulses&7)) {
          current_byte = getByte();
        } else {
          current_byte <<= 1;
        }
        current_sample = current_byte&128;
        OCR1A = symbols[0];
        break;
      case DATA:
        if(!(pulses&15)) {
          current_byte = getByte();
        } else {
          if(!(pulses&1)) {
            current_byte <<= 1;
          }
        }
        OCR1A = symbols[bool(current_byte&128)];
        break;
      case TONE:
        OCR1A = symbols[0];
        break;
      case PAUSE:
        OCR1A = PAUSEMS;
      }
    pulses--;
  
  }
#endif
