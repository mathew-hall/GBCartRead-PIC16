/**
 * Miscellany for simulating building for PICs on non-PIC targets.
 * (c) 2012 Mathew Hall.
 * This work is licensed under a Creative Commons Attribution-NonCommercial 3.0 Unported License.
http://creativecommons.org/licenses/by-nc/3.0/
 */
#ifndef __base_h__
#define __base_h__

typedef unsigned char  uchar;
typedef unsigned short t_address;


#define nop()   asm("nop");
#define _XTAL_FREQ 4000000UL


#ifdef __amd64
//TODO: add more compiler-generated flags set for x86 et al.
#define TARGET_PC 1
#endif

#ifdef TARGET_PC


#define __CONFIG(x) //x

#define __delay_us(x) sleep(x/100)
#define __delay_ms(x) sleep(x/10)


extern unsigned char TXREG;
extern unsigned char TXIF;
extern unsigned char RCIF;
extern unsigned char RCREG;

extern unsigned char SRCLR, PORTA,PORTB, PORTC, TRISA, TRISB, TRISC, INTCON, ANSEL, ANSELH;
extern unsigned char RA0,RA1,RA2,RA3,RA4,RA5,RA6,RA7;
extern unsigned char RB0,RB1,RB2,RB3,RB4,RB5,RB6,RB7;
extern unsigned char RC0,RC1,RC2,RC3,RC4,RC5,RC6,RC7;

unsigned char SPBRG, RCSTA, TXSTA;
#else

#include <htc.h>
#include "pic.h"
#endif

#endif
