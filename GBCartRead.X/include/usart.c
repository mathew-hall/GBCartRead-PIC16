/**
 * UART stuff. This code is based off any implementation you might find.
 * Maps to stdio functions when built on AMD64.
 */
#include "base.h"

#include <stdio.h>
#include "usart.h"


#ifdef __amd64
void putch(unsigned char byte){
	putchar(byte);
}

unsigned char getch(){
	return getchar();
}

unsigned char getche(){
	unsigned char in = getchar();
	putchar(in);
	return in;
}

#else
void 
putch(unsigned char byte) 
{
	//output byte
	while(!TXIF)	//wait until txif nonempty
		continue;
	TXREG = byte;
}

unsigned char 
getch() {
	while(!RCIF)	//wait til rcif is nonempty
		continue;
	return RCREG;	
}

unsigned char
getche(void)
{
	unsigned char c;
	c = getch();
	putch(c);
	return c;
}
#endif 
