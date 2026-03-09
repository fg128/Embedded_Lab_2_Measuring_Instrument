/*  This program demonstrates how to use interrupts and a hardware timer.
	It uses a bit variable as a flag, to allow the interrupt service routine
	and the foreground program to communicate in a safe way.   */

#include <ADUC841.H>
#include "typedef.h"
#include "adc_mode.h"


#define PERIOD		-250		// 250 clock cycles interrupt period
/* This negative value will be cast as an unsigned 8-bit integer below, and
   this will result in a timer reload value of 6.  Counting up from that value,
   the 8-bit timer will overflow after 250 clock cycles.  At a clock frequency
   of 11.0592 MHz, that gives interrupt frequency 44.2368 kHz, period ~22.6 us.  */

#define NUM_INTS	44240		// number of interrupts between events
// This value gives event frequency almost exactly 10 Hz, period 0.1 s

// These are global variables: static and available to all functions
sfr16 RCAP2 = 0xCA;                 // Timer 2 reload register, 16-bit
bit		period_over;		        // global variable - flag to signal event
uint16	period_count;		        // global variable to count interrupts
uint32  schmitt_count;	            // global variable - number of timer2 interrupts in one period
uint16  frequency;		            // global variable - estimated frequency in Hz


// Timer 0 generates counts to 0.1s and generate an pulse
void timer0 (void) interrupt 1 		    // interrupt vector at 000BH
{
    // Taken from blinky-timer-2.c
    period_count++;					// increment interrupt counter
    if (period_count >= NUM_INTS) 	// if enough interrupts have been counted
    {
        period_count = 0;			// reset the counter
        period_over  = 1;			// set the flag - 0.1s has passed

				// Turn off the timer
        TR0 = 0;
    }
}


// Timer 2 counts the number of schmitt trigger edges in the input signal during each 0.1s period.
void timer2 (void) interrupt 5
{
    schmitt_count++;                // increment the schmitt edge count for this period
    TF2 = 0;					        // clear interrupt flag
}	// end timer2 interrupt service routine


void setup_frequency_timers()
{
    period_over = 0;		            // initialize the flag to 0
    period_count = 0;		            // initialize the interrupt counter to 0
    schmitt_count = 0;              // initialize the schmitt edge count to 0
    frequency = 0;		              // initialize the frequency variable to 0

    // Set up the timer 0 interrupt (Taken from blinky-timer-2.c)
    TH0   = (unsigned char) PERIOD;		// set timer period
    TL0   = (unsigned char) PERIOD;
    TMOD |= 0x02;		                // select mode 2
    TR0   = 0;			                // OFF timer 0 (should be off until we start counting)
    ET0   = 1;			                // enable timer 0 interrupt
    EA    = 1;			                // global interrupt enable

    // Set up timer 2 in timer mode, auto reload, and external control (Taken from FreqGen_timer2-2.c)
    T2CON = 0x06;                       // all zero except run control
    ET2   = 0;                          // disable timer 2 interrupt
    RCAP2 = 0xFFFF;                     // Set reload value to maximum, so it will overflow on every schmitt trigger edge
}


uint16 get_frequency_value()
{
    TR0   = 1;			    // Start timer 0
    ET2   = 1;              // enable timer 2 interrupt
	P1 		= 0x00;         // Start P1
    while(!period_over);    // Wait for the timer to finish
    ET2   = 0;              // Turn off timer 2 interrupt

	frequency = schmitt_count;	// calculate frequency in Hz, based on period count and interrupt period
    schmitt_count = 0;	        // reset the schmitt count for the next 0.1s period
    period_over = 0;            // reset the period over flag for the next period

    return frequency;
}


//There can be issue with this if the input voltage is negative,
//the circuit should insure this dos not happen but idk, will have to test
uint16 get_amplitiude_value()
{
	// ADC value is always positive, so we can initialize min to max value,
	// and max to min value, and then update them as we read values from the ADC.
    uint16 adc_max = 0;
    uint16 adc_min = 0xFFFF;
    uint32 peak    = 0;
    uint16 adc_value = 0;

    // Select adc channel 1
    ADCCON2 = 0x01;

    // Start timer 0
    TR0 = 1;

    // While timer 0 is counting
    while(!period_over)
    {
        adc_value = get_adc_value(1); //with an averaging of 5, this in theory should be 55 KHz

        // Compare previous adc_value
        adc_max = adc_value > adc_max ? adc_value : adc_max;
        adc_min = adc_value < adc_min ? adc_value : adc_min;
    }
	period_over = 0;
    
    // Need to double check this, some nuances with the circuit
    peak = 2*(adc_max - adc_min);

    // Convert peak to mv
	  peak = peak * 2500L/4096L;
    return (uint16) peak;
}

// Gets DC value in mv
uint16 get_mDC_value()
{
    uint16 adc_value = 0;
    uint32 mv = 0;

    // Select adc channel 0
    ADCCON2 = 0x02;

    adc_value = get_adc_value(10);

	  // Convert raw value to mV
	  mv = adc_value * 2500L/4096L;
    return (uint16) mv;
}
