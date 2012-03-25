/*
 * GBCartRead PIC Port.
 * PIC port (c) 2012 Mathew Hall
 * Parts based on GBCartRead: (c) 2011 by insideGadgets
 * http://www.insidegadgets.com
 * This work is licensed under a Creative Commons Attribution-NonCommercial 3.0 Unported License.
 * http://creativecommons.org/licenses/by-nc/3.0/
 */


#include <stdio.h>
#include "include/base.h"
#include "include/shiftreg.h"
#include "include/usart.h"


__CONFIG(0x3FD4); //MCLR as input, WDT off, INTRC + OSCOUT as IOs.

//prompts:
#define DUMP_RAM        'A'
#define RUMP_ROM        'O'
#define RUN_DIAGNOSTICS	'Z'
#define INTERACTIVE     'i'

//default time to wait between loading and read/writing. Reduce this to go faster and
//increase likelihood of errors.
#define DELAY_TIME      50


//WR, RD and MREQ are active low, thus, OFF and ON are counterintuitive.
//Don't use these for anything other than those 3 pins.
#define OFF 1
#define ON 0





//Pin assignment for cartridge control lines:
#define WR      RA0
#define RD      RB4
#define MREQ    RB6



uchar mcb_type;
uchar in;

unsigned int rom_banks = 2;
uchar ram_banks = 1;
uchar cart_type = 1;
uchar rom_sz = 1;
uchar ram_sz = 1;

void init(){
    //Disable interrupts, configure pins for digital IO.
    INTCON=0;

    TRISA=0b00001000; //MCLR is always input
    TRISB=0b10100000; //PORTB.7 = TX, 5 = RX, 4 & 6 are outs
    TRISC=0b11111111;
    ANSEL=0x00;
    ANSELH = 0x00;
    PORTA=0;

    /* setup shift registers:*/
    SRCLR = 1;
    SER = 0;
    TOGGLE_CLOCK(SRCK);
    TOGGLE_CLOCK(SRCK);

    RCLR = 1;
    TOGGLE_CLOCK(RCK);
    TOGGLE_CLOCK(RCK);




    //turn all cartridge control pins off:
    RD = OFF;
    WR = OFF;
    MREQ = OFF;


    //Initialise UART, see datasheet for SPBRG value:
    SPBRG = 0x19;
    TXSTA = 0b00100100;
    RCSTA = 0b10010000;


}



/**
 * Shifts addr out and reads data from data register of cart.
 * @param addr the address to read
 * @return the byte at that address
 */
uchar read_address(t_address addr){

    shift_out(addr);

    RD = ON;
    __delay_us(DELAY_TIME);
    uchar in = PORTC;
    RD = OFF;

    return in;

}

/**
 * Write data at addr.
 * @param addr address to clobber with data
 * @param data data to put at address
 */
void write_address(t_address addr, uchar data){//, int* pin){

    shift_out(addr);
    TRISC = 0x0; //C is now outputs
    PORTC = data;
    WR = ON;
    __delay_us(DELAY_TIME);
    WR = OFF;
    TRISC = 0xff;
}

/**
 * Read from RAM at addr. Difference between ROM and RAM reads is the MREQ pin toggle.
 * @param addr the address of RAM to read
 * @return the byte at that address.
 */
uchar ram_read_address(t_address addr){
    shift_out(addr);
    MREQ = ON;
    RD = ON;
    __delay_us(DELAY_TIME);
    uchar in = PORTC;
    MREQ = OFF;
    RD = OFF;

    return in;

}

/**
 * Write data at addr.
 */
void ram_write_address(t_address addr, uchar data){
    shift_out(addr);
    TRISC = 0x0;
    PORTC = data;
    MREQ = ON;
    WR = ON;
    __delay_us(DELAY_TIME);
    MREQ = OFF;
    WR = OFF;
    TRISC = 0xff;
}

//bank switching is always a case of writing bank at some address.
//address depends on ROM or RAM.
#define ram_bank_select(n) write_address(0x4000,n)
#define rom_bank_select(n) write_address(0x2100,n)

/**
 * Reads metadata from the cart. This HAS to be done before you do anything else
 * and will be unless you modify main().
 */
void read_descriptor(){
    printf("\nN:");
    for(int addr= 0x0134; addr <= 0x0142; addr++){
        putch(read_address(addr));
    }
    putch('\n');
    
    cart_type = read_address(0x0147);
    printf("\nCT:%x\n",cart_type);


    rom_sz = read_address(0x0148);
    printf("RoS:%x\n",rom_sz);
    rom_banks = rom_banks << rom_sz;
	if(cart_type <= 3){
		if(rom_sz == 5){
			rom_banks = 63;
		}else if(rom_sz == 6){
			rom_banks = 125;
		}
	}else if(rom_sz == 82){
		rom_banks = 72;
	}else if(rom_sz == 83){
		rom_banks = 80;
	}else if(rom_sz == 84){
		rom_banks = 96;
	}



    ram_sz = read_address(0x0149);

    if(ram_sz == 3){
        ram_banks = 4;
    }

    printf("RaS:%x\nRaB:%d\nRoB:%d\n",ram_sz,ram_banks,rom_banks);
}


void dump_ram(){
    t_address end_address = 0xffff;

    if(cart_type == 6 && ram_sz == 0)   { end_address = 0xa1ff; }
    if(ram_sz == 1)                     { end_address = 0xa7ff; }
    if(ram_sz == 2 || ram_sz == 3)      { end_address = 0xbfff; }


    //MBC2 fix:
    read_address(0x0134);

    if(end_address == 0xffff) return; //don't bother reading RAM if there isn't any.

    //init MBC:
    write_address(0x0,0x0A);
    printf("RAM <<<");
    for(int bank =0; bank < ram_banks; bank++){
        //select RAM bank by writing bank number at 0x4000
        ram_bank_select(bank);

        for(t_address addr = 0xa000; addr <= end_address; addr++){
            putch(ram_read_address(addr));
        }
    }

    write_address(0x0,0);

    //magic numbers are boring, here's a nice constant instead:
    printf(">>> End of RAM.\n");


}

void dump_rom(){
    printf("ROM dump from <<<");

    uchar bank = 0;
    
    for(bank = 1; bank < rom_banks; bank++){
        rom_bank_select(bank);
        t_address addr = 0;
        if(bank > 1){
            addr = 0x4000;
        }
        for(; addr <= 0x7FFF; addr++){
            putch(read_address(addr));

        }


    }
    printf(">>> End of ROM.\n");

}

/**
 * This is useful for tracking down sources of disappearing bytes.
 * e.g. backspace character - PuTTY doesn't log BKSPC, it deletes the previous byte
 * from the log. Hex dumping from the device normally catches this.
 */
void diag_hexdump(){
    printf("Diagnostic hexdump mode: first 512 bytes:");

    rom_bank_select(1);
    for(t_address addr = 0; addr <= 512; addr++){
        if(addr%16 == 0){
            printf("\n@%x:",addr);
        }
        printf("0x%x ",read_address(addr));
    }
}

void diagnostics(){
printf("Diag: Cart MUST be disconnected. Any key to go\n_");
        getch();
        printf("SR test: 0x1\n");
        shift_out(0x1);
        getch();
        printf("SR test: 0xffff\n");
        shift_out(0xffff);
        getch();
        printf("SR test: 0x0\n");
        shift_out(0x0);
        getch();

        printf("3 PORTC reads\n_");
        getch();
        printf("%x\n",read_address(0x0));
        getch();
        printf("%x\n",read_address(0x0));
        getch();
        printf("%x\n",read_address(0x0));

        printf("PORTC write to 0xff\n_");
        getch();
        write_address(0x0, 0xff);
        getch();

        printf("Toggle WR RD MREQ\n_");
        getch();
        RD = ON;
        WR = ON;
        MREQ = ON;
        printf("Changing to OFF (logical 1)\n_");
        getch();

        RD = OFF;
        WR = OFF;
        MREQ = OFF;

        printf("\nDiagnostics complete.\n");

}

void interactive(){
    printf("\nInteractive Mode, any key to go or X to quit:\n");
    t_address ad = 0x0000;


    in = getche();

    while(1){

        printf("\n[SRHX]_");
        in = getche();
        int num = 12; //3 * 4 -

        if(in == 'S'){
            printf("\nEnter address in 4b16:_");
            ad = 0x0000;
            for(num = 12; num >= 0; num-=4){
                in = getche();
                if(in >= 'A' && in <= 'F'){
                    in = in - 55;
                }else if(in >= '0' && in <= '9'){
                    in = in - 48;
                }else{
                    printf("\nError, %c is out of range 0-F",in);
                    continue;
                }

                ad |= (in << num);

            }
            printf("\nSetting address to %x",ad);
        }else if(in == 'R'){
            printf("\nRead @%x: %x\n",ad,read_address(ad));
        }else if(in == 'H'){
            diag_hexdump();
        }else if(in == 'X'){
            read_address(0x0);
            printf("\nExit\n");
            return;
        }


    }
}

#ifdef __amd64
//Building something interactive? main will be called pic_main to make it easier.
int pic_main(void){
#else
int main(void) {
#endif
    init();

    printf("READY\nLoad?");


    //wait for input:
    in = getche();

    read_descriptor();
    while(1){
        printf("\n[OAZi]?_");
            in = getche();
            if(in == DUMP_RAM){
                    printf("RaD:\n");
                    dump_ram();
            }else if(in == RUMP_ROM){
                    printf("RoD:\n");
                    dump_rom();
            }else if(in == RUN_DIAGNOSTICS){
                diagnostics();
            }else if (in == INTERACTIVE){
                interactive();
            }
    }
    return 0;
}
