/**
 * Driver for 74HC594 (note the 4! This isn't for 595s!) shift registers.
 * (c) 2012 Mathew Hall.
 * This work is licensed under a Creative Commons Attribution-NonCommercial 3.0 Unported License.
http://creativecommons.org/licenses/by-nc/3.0/
 */


#include "base.h"

//Flips a pin on and off, nothing more.
#define TOGGLE_CLOCK(ck)\
ck = 0; \
nop(); \
ck = 1; \
nop(); 

//Set these based on PIC and circuit layout.
//I've tied RCLR and SRCLR together to save a pin, they can be separate but the
//behaviour _should_ be the same (not tested, not guaranteed).
#define SRCLR   RA4
#define RCLR    SRCLR
#define SER     RA5
#define SRCK    RA2
#define RCK     RA1

void shift_out(t_address addr);
