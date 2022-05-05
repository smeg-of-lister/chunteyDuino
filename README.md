# chunteyDuino
Alternative firmware for ArduiTape

## Usage

 * Place a .TZX/CDT file in the root of a FAT formatted SD card. Make sure you rename the file to match the one defined in the chunteyDuino.ino file. (see [notes](#notes))
 * Compile the code and upload to your ArduiTape/TZXDuino, etc. using the Arduino IDE.
 * The file should playback automatically at power up/reset, so you will need to either connect power as needed, or use the reset button.
 * If the file does not play back correctly, it is likely due to the file containing a block type that is not yet implemented. (see [here](#currently-implemented-tzx-block-types))


### Required Libraries
[SdFat 1.1.4](https://github.com/greiman/SdFat/tree/1.1.4)

## Background
chunteyDuino started out back in 2019 as a proof of concept brought about by my desire to play TZX files generated by [Proyecto O.T.L.A.](https://code.google.com/archive/p/otla/) using an [ArduiTape](http://arduitape.blogspot.com/). I found that the standard TZXDuino firmware was unable to play such files correctly, so I wrote a barebones re-implementation and _eventually_ managed to get O.T.L.A encoded files playing satisfactorily. The firmware could only play a very limited subset of TZX blocks, couldn't interact with the buttons or a display, and could only play a single file defined in the source code. I had achieved what I set out to do and my interest in the project waned - At this point, I just left chunteyDuino to gather dust...

Fast-forward to August 2021 and I encountered a [YouTube video](https://www.youtube.com/watch?v=MAIsOIwgJWA) by [Noel's Retro Lab](https://www.youtube.com/channel/UC2-SP1bYi3ueKlVU7I75wFw), in which Noel experiments with pushing the limits of tape loading on the Amstrad CPC. I noticed he ran into the same issues that I encountered when trying to play back extremely fast tape loader data. So I contacted Noel and offered up chunteyDuino as a donation to his [HyperLoader project](https://github.com/llopis/HyperLoader). I had not intended to revisit it myself, but I just couldn't resist firing up my CPC and seeing how fast HyperLoader could go using chunteyDuino. So I implemented the TZX blocks required to play HyperLoader's output and gave it a go. The results were very impressive and my interest in the project was revitalized!

## Implementation
At the time of writing, chunteyDuino consists of two main parts:-
1. The file processor - which reads the input file from the SDCard, parses the file structure (currently only TZX/CDT files), converts to an simplified internal format and fills a FIFO buffer. This runs in a loop, checking if the buffer is filled and fetching more data from the SD card if it is not.

2. The pulse generator - this is an interrupt routine which reads from the FIFO buffer, parses the internal format and generates digital pulses on the microcontroller's output pin.

## Internal Format
Input files are translated into a simplified internal format by the file processor, which is then sent via a FIFO buffer to the pulse generator ISR. The idea of this approach is to balance the amount of data passing though the buffer with the complexity of the ISR, so that the buffer is space-effective without drastically increasing the processing time spent in the ISR.

The internal format consists of a series of blocks, which in turn consist of a single byte defining the type, followed by parameters (if applicable) and then block data from the input file (if applicable).

#### Block Types

Type|Name|TZX Equivalent|Parameters|Description
:--:|:--:|:------------:|:---------|:----------
0x00|NONE|N/A|_none_|As you would expect, this does absolutely nothing. The buffer is filled with this block type just before the start of playback.
0x01|STOP|ID20 (zero length)|_none_|Stops playback. The signal level is set low and the interrupt timer is disabled.
0x02|TONE|ID12|_pulse count_, _pulse length_|Plays a continuous tone of a fixed pulse length.
0x03|PULSES|ID13|_pulse count_|Plays a series of individual pulses of varying lengths.
0x04|DATA|ID14|_pulse count_, _pulse length zero_, _pulse length one_, _last byte adjust_|Plays a binary frequency-shift keyed (BFSK) signal.
0x05|PAUSE|ID20 (non-zero length)|_millisecond count_|Sets the signal level low for a specified length of time.
0x06|SAMPLE|ID15|_pulse count_, _sample period_, _last byte adjust_|Plays a stream of ones and zeros representing the signal level (0 = low, 1 = high) with a specified sample period.

Pulse lengths are counted in clock cycles at the 16Mhz of the Arduino Nano, therefore the pulse length for PAUSE blocks is 16000.

A standard ZX Spectrum ROM header block (as in TAP files or TZX ID10) would consist of:-

* The pilot tone  
  **TONE: 8063 pulses, pulse length of 9911**
* Two sync pulses  
  **PULSES: 2 pulses, first pulse length 3430, second pulse length 3780**
* The data. The header is 19 bytes and each byte is represented by 16 pulses  
  **DATA: 304 pulses, zero pulse length 3909, one pulse length 7817, no last byte adjust**
* A one second pause  
  **PAUSE: 1000 milliseconds**

#### Block Structures

##### 0x00: NONE
Offset|Value|Type|Description|Notes
:----:|:---:|:--:|:----------|:----
0x00|0x00|BYTE|Block type| |

##### 0x01: STOP
Offset|Value|Type|Description|Notes
:----:|:---:|:--:|:----------|:----
0x00|0x01|BYTE|Block type| |

##### 0x02: TONE
Offset|Value|Type|Description|Notes
:----:|:---:|:--:|:----------|:----
0x00|0x02|BYTE|Block type| |
0x01| |WORD|Pulse count| |
0x03| |WORD|Pulse length| |

##### 0x03: PULSES
Offset|Value|Type|Description|Notes
:----:|:---:|:--:|:----------|:----
0x00|0x03|BYTE|Block type| |
0x01|N|BYTE|Pulse count|This will likely be changed to WORD at some point
0x02| |WORD\[N\]|Stream of pulse lengths| |

##### 0x04: DATA
Offset|Value|Type|Description|Notes
:----:|:---:|:--:|:----------|:----
0x00|0x04|BYTE|Block type| |
0x01|N|BYTE\[3\]|Pulse count| |
0x04| |WORD|Pulse length of zero bit| |
0x06| |WORD|Pulse length of one bit| |
0x08| |BYTE|Adjustment for pulse count if last byte of data is not fully used|This is a bit kludgy. Will most likely change this so the adjustment to the pulse count is performed _before_ placing it in the buffer
0x09| |BYTE\[N/16\]|Data| |

##### 0x05: PAUSE
Offset|Value|Type|Description|Notes
:----:|:---:|:--:|:----------|:----
0x00|0x05|BYTE|Block type| |
0x01| |WORD|Millisecond count| |

##### 0x06: SAMPLE
Offset|Value|Type|Description|Notes
:----:|:---:|:--:|:----------|:----
0x00|0x06|BYTE|Block type| |
0x01|N|BYTE\[3\]|Pulse count| |
0x04| |WORD|Sample period| |
0x06| |BYTE|Adjustment for pulse count if last byte of data is not fully used|This is a bit kludgy. Will most likely change this so the adjustment to the pulse count is performed _before_ placing it in the buffer
0x07| |BYTE\[N/8\]|Samples| |

## STM32 Blue Pill version

I currently use a STM32F103C8 Blue Pill to develop chunteyDuino as I can use its USART to send debug information back to a serial console (the Arduino Nano isn't fast enough to do serial and reliably play back pulses at the same time). From my experiences, both the ZX Spectrum and Amstrad CPC seem happy loading from a 3.3v digital signal via their cassette ports. See the diagram below for connections. 

![](chunteyDuino_bluepill.png)

#### Arduino IDE configuration

You will need to install the STM32Duino core in the Arduino IDE and preferably the STM32 HID Bootloader on your BluePill board. A fantastic guide on how to do this is available [here](https://www.youtube.com/watch?v=Myon8H111PQ).

### Currently Implemented TZX Block Types

ID|Description|Notes
:-:|:----------|:----
**ID10**|**Standard speed data block**|
**ID11**|**Turbo speed data block**|
**ID12**|**Pure tone**|
**ID13**|**Sequence of pulses of various lengths**|
**ID14**|**Pure data block**|
**ID15**|**Direct recording block**|
~~ID18~~|~~CSW recording block~~|_Standard RLE blocks should be possible to implement, but playing zlib compressed blocks on an ATMEGA328P? No chance!_
~~ID19~~|~~Generalized data block~~|_Impossible to fully implement due to the memory constraints of the ATMEGA328P, although a limited implementation should be possible_
**ID20**|**Pause (silence) or 'Stop the tape' command**|
_ID21_|_Group start_|_Partial - the 'group name' string is currently ignored_
**ID22**|**Group end**|
~~ID23~~|~~Jump to block~~|
~~ID24~~|~~Loop start~~|
~~ID25~~|~~Loop end~~|
~~ID26~~|~~Call sequence~~|
~~ID27~~|~~Return from sequence~~|
~~ID28~~|~~Select block~~|
~~ID2A~~|~~Stop the tape if in 48K mode~~|
~~ID2B~~|~~Set signal level~~|
_ID30_|_Text description_|_Partial - the text is currently ignored_
~~ID31~~|~~Message block~~|
~~ID32~~|~~Archive info~~|
~~ID33~~|~~Hardware type~~|
~~ID35~~|~~Custom info block~~|
**ID5A**|**"Glue" block (90 dec, ASCII Letter 'Z')**|

### Notes

Please note that this firmware is currently **_extremely_** barebones, and does not interact with the ArduiTape's buttons or screen. It reads data from a fixed file defined in the source code - so if you plan on experimenting with it, **please bear in mind it is better to rename your input file on the SD Card than to repeatedly flash your Arduino using different filenames**. I accept no responsibility if you brick your device doing so!
