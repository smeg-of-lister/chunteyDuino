# chunteyDuino
Alternative firmware for ArduiTape

### Required Libraries
[SdFat 1.1.4](https://github.com/greiman/SdFat/tree/1.1.4)

### Background
chunteyDuino started out back in 2019 as a proof of concept brought about by my desire to play TZX files generated by [Proyecto O.T.L.A.](https://code.google.com/archive/p/otla/) using an [ArduiTape](http://arduitape.blogspot.com/). I found that the standard TZXDuino firmware was unable to play such files correctly, so I wrote a barebones re-implementation and _eventually_ managed to get O.T.L.A encoded files playing satisfactorily. All the firmware could do was play a very limited subset of TZX blocks, couldn't interact with the buttons or a display, and could only play a single file defined in the source code. I had achieved what I set out to do and my interest in the project waned - At this point, I just left chunteyDuino to gather dust...

Fast-forward to August 2021 and I encountered a [YouTube video](https://www.youtube.com/watch?v=MAIsOIwgJWA) by [Noel's Retro Lab](https://www.youtube.com/channel/UC2-SP1bYi3ueKlVU7I75wFw), in which Noel experiments with pushing the limits of tape loading on the Amstrad CPC. I noticed he ran into the same issues that I encountered when trying to play back extremely fast tape loader data. So I contacted Noel and offered up chunteyDuino as a donation to his [HyperLoader project](https://github.com/llopis/HyperLoader). I had not intended to revisit it myself, but I just couldn't resist firing up my CPC and seeing how fast HyperLoader could go using chunteyDuino. So I implemented the TZX blocks required to play HyperLoader's output and gave it a go. The results were very impressive and my interest in the project was revitalized!

### Implementation

At the time of writing, chunteyDuino consists of two main parts:-

1. The file processor - which reads the input file from the SDCard, parses the file structure (currently only TZX/CDT files), converts to an simplified internal format and fills a FIFO buffer. This runs in a loop, checking if the buffer is filled and fetching more data from the SD card if it is not.

2. The pulse generator - this is an interrupt routine which reads from the FIFO buffer, parses the internal format and generates digital pulses on the ArduiTape's output pin.

## Internal Format

_placeholder_

## STM32 Blue Pill version

_placeholder_

### Notes

Please note that this firmware is currently **_extremely_** barebones, and does not interact with the ArduiTape's buttons or screen. It reads data from a fixed file defined in the source code - so if you plan on experimenting with it, **please bear in mind it is better to rename your input file on the SD Card than to repeatedly flash your Arduino using different filenames**. I accept no responsibility if you brick your device doing so!

