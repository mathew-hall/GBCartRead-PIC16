/**
 * Miscellany for simulating building for PICs on non-PIC targets.
 * (c) 2012 Mathew Hall.
 * This work is licensed under a Creative Commons Attribution-NonCommercial 3.0 Unported License.
 http://creativecommons.org/licenses/by-nc/3.0/
 */

#include "base.h"

#ifdef TARGET_PC
unsigned char TXREG;
unsigned char TXIF;
unsigned char RCIF;
unsigned char RCREG;

unsigned char SRCLR, PORTA,PORTB, PORTC, TRISA, TRISB, TRISC, INTCON, ANSEL, ANSELH;
unsigned char RA0,RA1,RA2,RA3,RA4,RA5,RA6,RA7;
unsigned char RB0,RB1,RB2,RB3,RB4,RB5,RB6,RB7;
unsigned char RC0,RC1,RC2,RC3,RC4,RC5,RC6,RC7;

unsigned char SPBRG, RCSTA, TXSTA;
#endif