/*  This program demonstrates how to use interrupts and a hardware timer.
	It uses a bit variable as a flag, to allow the interrupt service routine
	and the foreground program to communicate in a safe way.   */

#include <ADUC841.H>
#include "typedef.h"
#include "display.h"

#define PERIOD		-250		// 250 clock cycles interrupt period
/* This negative value will be cast as an unsigned 8-bit integer below, and
   this will result in a timer reload value of 6.  Counting up from that value,
   the 8-bit timer will overflow after 250 clock cycles.  At a clock frequency
   of 11.0592 MHz, that gives interrupt frequency 44.2368 kHz, period ~22.6 us.  */

#define NUM_INTS	11059		// number of interrupts between events
// This value gives event frequency almost exactly 4 Hz, period 0.25 s

#define LED_BIT		0x10		// position of LED on port 3

#define ADC_MODE		1
#define HERTZ_MODE		2
#define AMPLITUDE_MODE	4

// These are global variables: static and available to all functions
unsigned int	TimerTick;		// global variable to count interrupts
bit				TimeOver;		// global variable - flag to signal event

typedef struct {
	uint32 value;
	uint16 offset;
	uint16 gain;
	uint32 mV;
} ADC;

ADC adc;

/*------------------------------------------------
Interrupt service routine for timer 0 interrupt.
"Called" by the hardware when the interrupt occurs.
------------------------------------------------*/
void timer0 (void) interrupt 1 		// interrupt vector at 000BH
{
	TimerTick++;					// increment interrupt counter
	if (TimerTick > NUM_INTS) 		// if enough interrupts have been counted
	{
		TimerTick = 0;				// reset the counter
		TimeOver  = 1;				// set the flag - event has occurred
	}
}

void adc_calibrate()
{
    ADCCON3 = 0x01; // Calibrate offset
	while((ADCCON3 & 0x01) == 0x01); // Wait until calibration is done
	adc.offset = ((ADCOFSH & 0x3F) << 8) | ADCOFSL;

    ADCCON3 = 0x03; // Calibrate gain
	while((ADCCON3 & 0x01) == 0x01); // Wait until calibration is done
	adc.gain = ((ADCGAINH & 0x3F) << 8) | ADCGAINL;
}

void adc_setup()
{
    ADCCON1 = 0xB0;
    ADCCON2 = 0x01; // Set to channel 1 output
	adc_calibrate();
}

uint16 get_adc_mv_value()
{
	uint16 raw, mv;
    ADCCON2 |= 0x10; // Trigger single conversion (Sets SCONV, Bit 4)

	// Wait for ADCI flag (Bit 7) to become 1, meaning conversion is done
    while((ADCCON2 & 0x10) == 0x10);
	// ADCCON2 &= ~0x80; // Manually clear the ADCI flag

	// Read the 12-bit value raw from the ADC data registers
	raw = ((ADCDATAH & 0x0F) << 8) | ADCDATAL;

	// Convert raw value to mV
	mv = raw * 2500L/4096L;
	return mv;
}

void main (void)
{
	uint8 mode;
	uint16 value;

	// Setup adc and display settings before going into the main loop
    adc_setup();
	display_setup();

	// After setting up, main goes into an infinite loop
	while (1)
	{
		mode = P2 & 0x07; // Read the 3 switch bits (P2.0, P2.1, P2.2)

		switch(mode)
		{
			case ADC_MODE: // First switch on, read ADC value in mV mode
				value = get_adc_mv_value();
				break;
			case HERTZ_MODE: // Second switch on, read hertz value
				value = get_hertz_value();
				break;
			case AMPLITUDE_MODE: // Third switch on, read amplitude value
				value = get_amplitude_value();
				break;
			default: // All switches off, or more than one switch on, display 0
				value = 0;
		}

		display(value, mode);
	}
}
