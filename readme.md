# GBCartRead-PIC16
A PIC16-based Game Boy ROM dumper. Code is loosely based on [InsideGadgets' Arduino implementation](http://www.insidegadgets.com/projects/gbcartread-gameboy-cart-reader/) for technical details, see that site.

Includes schematic for the hardware and source (HI-TECH PIC C) for the firmware.

# Required Hardware
* Gameboy cartridge header (or solder a pin header to your cartridge)
* PIC16F690 (any PIC with enough IOs - 15, including 8 bidirectional - and hardware UART should work)
* 2x 74xx594 shift registers (595s will work with changes to code)
* MAX232 or equivalent level shifter
* Handful of passive components (4x 1uF capacitors, 4x 10Kohm reistors)
* 5V regulated power supply
* a serial port

# Building
Ideally it should be built in MPLAB (or MPLAB X), but the supplied makefile *should* work standalone.

1. Load project in MPLAB X
2. Configure compiler path if necessary
3. Run Build Clean

# Using
1. Flash the hex to the PIC
2. Connect the cartridge and serial cable
3. Run some terminal program on the COM port: 9600 baud, 8 bits, no parity, 1 stop bit
4. Switch everything on
5. Use menu

## Menu Options
* O: dump rOm
* A: dump rAm
* Z: run diagnoZtics
* i: interactive mode

### Diagnostics
Performs a few actions that you can test, ideal for making sure your hardware works.
Runs through several writes/reads to different parts, showing the expected result. Hit any key at each prompt to advance to the next test.
*Critically, WR should be tested before attempting a RAM dump.* Pokemon have been lost due to a faulty WR pin.

### Interactive Mode
* S: set address lines to the following value (reads 4 hex digits, uppercase only).
* R: read cartridge at current address.
* H: hexdump the first 512 bytes of the cartridge.
* X: go back to the main menu.

# Code 
* base.h deals with PIC-specific stuff.
* shiftreg.h & shiftreg.c handle driving a '594 shift register (two in this case)
* usart.h & usart.c handle comms.
* main.c does everything else.

The #ifdef __amd64 is used to inject alternate implementations or define PIC-specific variables for AMD64 targets. This allows tests to be run on a PC rather than having to flash and run on a real PIC.

# Issues
* Avoid using loggers that log terminal content rather than raw input. PuTTY's logger will interpret backspace characters rather than writing them, which results in missing data. [RealTerm](http://realterm.sourceforge.net/) works fine for this. Future work is to modify InsideGadgets' Python script to interoperate with this port.
* Apology to EEs: Decoupling capacitors are omitted from the schematic. These should be included for reliability.
* MAX3232s are more expensive than MAX232s. A MAX232 or 202 should work fine in place of the 3232.
* Writing RAM isn't supported yet.
* It's slow. The delay between address writes and data read/writes can be reduced to increase speed, but ultimately the limit will be set by the serial port. The UART can be driven faster - change the values in init() using the table on page 164 of the [16F690 Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/41262E.pdf). I experienced strange behaviour past 9600 baud, and lacking an oscilloscope I decided to be patient rather than experiment blindly with decoupling capacitors.

# Credit & Licence
The reading and writing algorithms are taken from [InsideGadgets' implementation](http://www.insidegadgets.com/projects/gbcartread-gameboy-cart-reader/), licensed under the [Creative Commons Attribution-NonCommercial 3.0 Unported License](http://creativecommons.org/licenses/by-nc/3.0/). Copyright: (c) 2011 insideGadgets - http://www.insidegadgets.com

The remainder of the implementation, including all other functionality (unless otherwise indicated) is copyright (c) 2012 Mathew Hall. 

This work is licensed under a Creative Commons Attribution-NonCommercial 3.0 Unported License.
http://creativecommons.org/licenses/by-nc/3.0/
