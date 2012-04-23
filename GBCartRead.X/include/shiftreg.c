/**
 * Driver for 74HC594 (note the 4! This isn't for 595s!) shift registers.
 * (c) 2012 Mathew Hall.
 * This work is licensed under a Creative Commons Attribution-NonCommercial 3.0 Unported License.
http://creativecommons.org/licenses/by-nc/3.0/
 */

#include <stdio.h>
#include "shiftreg.h"
#include "base.h"

#ifdef TARGET_PC

void shift_out(t_address addr){
	printf("SR @ 0x%x",addr);
}

#else
void shift_out(t_address addr){	
    uchar count = 16;
    while(count-- != 0){
        addr = (addr << 1) | (addr >> 15) ;
        SER = addr & 1;
        TOGGLE_CLOCK(SRCK);
		
    }
    TOGGLE_CLOCK(RCK);
    TOGGLE_CLOCK(RCK);
    
}
#endif