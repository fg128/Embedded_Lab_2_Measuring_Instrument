#include <ADUC841.H>	// special function register definitions
#include "typedef.h"	// variable type definitions

#define PERIOD		-108    // 108 clock cycles interrupt period
#define HIGH_BYTE 	0xFF00	// mask to select upper byte of 16 bits
#define LOW_BYTE 	0xFF	// mask to select lowest byte
#define TOP_BIT		0x80	// mask to select bit 7
#define LOW_NIBBLE 	0x0F	// mask to select lowest 4 bits
#define LEDs		P0		// LEDs are connected to Port 0
#define SWITCHES  	P2		// switches are connected to Port 2
#define OUT			WR		// output on Port 3 pin 6 (called WR in header file

void main(){
    int sample = get_sample_from_adc();
}

int get_sample_from_adc(){

}

void timer0 (void) interrupt 1  // interrupt vector at 000BH
{
	static uint8 counter = 0;	// static => retains value between interrupts
	counter += Increment;		// advance the counter by specified increment
	if (counter < Increment)	// if the 8-bit counter has overflowed
		OUT = ~OUT;				// invert the output pin
}	// end timer0 interrupt service routine
