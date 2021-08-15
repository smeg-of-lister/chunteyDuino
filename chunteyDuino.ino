#include <SdFat.h>
#include "chunteyDuino.h"

uint8_t buffer[BUFFERSIZE];
uint16_t buffer_write, symbols[2];
volatile uint8_t current_signal = NONE, current_byte, last_byte_adjust = 0, next_signal = NONE;
volatile uint16_t buffer_read, current_sample, previous_sample;
volatile int32_t pulses = 0;
  
char fileName[17] = "hoh.cdt";

#if defined(ARDUINO_BLUEPILL_F103CB)
  HardwareTimer *Timer = new HardwareTimer(TIM3);
#endif

SdFat sd;
SdFile file;

void setup() {
  
  #if defined(ARDUINO_BLUEPILL_F103CB)
    Serial.begin(9600);
    while (!Serial);
  #endif

  #if defined(ARDUINO_BLUEPILL_F103CB)
    Timer->setPrescaleFactor(4);
    Timer->setMode(1, TIMER_OUTPUT_COMPARE_ACTIVE);
    Timer->setCaptureCompare(1, 1);
    Timer->attachInterrupt(1, output);
    Timer->refresh();  
  #else
    TCCR1A = 0;                                                                 
    TCCR1B = 0;
    SET(TCCR1B, CS10);
    SET(TCCR1B, WGM12);
  #endif

  if (!sd.begin(SPIPIN, SPISPEED)) {
    #if defined(ARDUINO_BLUEPILL_F103CB)
      sd.initError("sd:");
    #endif
    for (;;);
  }
  #if defined(ARDUINO_BLUEPILL_F103CB)
    sd.ls();
  #endif
  if (!file.open(fileName, O_READ)) {
    #if defined(ARDUINO_BLUEPILL_F103CB)
      sd.errorExit("file");
    #endif
    for (;;);
  }

  #if defined(ARDUINO_BLUEPILL_F103CB)
    pinMode(PB0, OUTPUT);
    HAL_GPIO_WritePin(EARPORT, EARPIN, GPIO_PIN_RESET);
  #else
    SET(DDRB, DDB1);
    CLEAR(EARPORT, EARPIN);
  #endif
  delay(1000);
  
}

void loop() {

  playFile();

}
