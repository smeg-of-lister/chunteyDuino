#define CLEAR(x,y)            (x&=(~(1<<y)))
#define SET(x,y)              (x|=(1<<y))
#define TOGGLE(x,y)           (x^=(1<<y))

#if defined(ARDUINO_BLUEPILL_F103CB)
  #define errorExit(msg)      errorHalt(F(msg))
  #define initError(msg)      initErrorHalt(F(msg))
#endif

#if defined(ARDUINO_BLUEPILL_F103CB)
  #define EARPIN              GPIO_PIN_8
  #define EARPORT             GPIOA
  #define SPIPIN              PB12
  #define SPISPEED            SD_SCK_MHZ(18)
#else
  #define EARPIN              PORTB1
  #define EARPORT             PORTB
  #define SPIPIN              PIN_SPI_SS
  #define SPISPEED            SPI_FULL_SPEED
#endif

#if defined(ARDUINO_BLUEPILL_F103CB)
  #define PILOTPULSE          11150
  #define SYNC1PULSE          3430
  #define SYNC2PULSE          3780
  #define ZEROPULSE           4397
  #define ONEPULSE            8794
  #define PAUSEMS             18000
#else
  #define PILOTPULSE          9911
  #define SYNC1PULSE          3049
  #define SYNC2PULSE          3360
  #define ZEROPULSE           3909
  #define ONEPULSE            7817
  #define PAUSEMS             16000
#endif
#define PILOTHEADERLENGTH     8063
#define PILOTDATALENGTH       3223

#define BUFFERSIZE            256

#define NONE                  0
#define STOP                  1
#define TONE                  2
#define PULSES                3
#define DATA                  4
#define PAUSE                 5
#define SAMPLE                6

#define ID10                  0x10              // standard speed data block
#define ID11                  0x11              // turbo speed data block
#define ID12                  0x12              // pure tone
#define ID13                  0x13              // pulse sequence
#define ID14                  0x14              // pure data block
#define ID15                  0x15              // direct recording block
#define ID20                  0x20              // pause or stop
#define ID21                  0x21              // group start
#define ID22                  0x22              // group end
#define ID30                  0x30              // text description
#define ID5A                  0x5a              // 'glue' block
#define EOF                   0xffff
