/*  This program demonstrates how to use interrupts and a hardware timer.
	It uses a bit variable as a flag, to allow the interrupt service routine
	and the foreground program to communicate in a safe way.   */

#include <ADUC841.H>
#include "typedef.h"


#define PERIOD		-250		// 250 clock cycles interrupt period
/* This negative value will be cast as an unsigned 8-bit integer below, and
   this will result in a timer reload value of 6.  Counting up from that value,
   the 8-bit timer will overflow after 250 clock cycles.  At a clock frequency
   of 11.0592 MHz, that gives interrupt frequency 44.2368 kHz, period ~22.6 us.  */

#define NUM_INTS	4424		// number of interrupts between events
// This value gives event frequency almost exactly 10 Hz, period 0.1 s

// These are global variables: static and available to all functions
sfr16 RCAP2 = 0xCA;                 // Timer 2 reload register, 16-bit
bit		period_over;		        // global variable - flag to signal event
uint16	period_count;		        // global variable to count interrupts
uint16  schmitt_count;	            // global variable - number of timer2 interrupts in one period
uint16  frequency;		            // global variable - estimated frequency in Hz


// Timer 0 generates counts to 0.1s and generate an pulse
void timer0 (void) interrupt 1 		    // interrupt vector at 000BH
{
    if (!period_over) // If period_over is already set, wait for timer2 to reset it
    {
        // Taken from blinky-timer-2.c
        period_count++;					// increment interrupt counter
        if (period_count >= NUM_INTS) 	// if enough interrupts have been counted
        {
            period_count = 0;			// reset the counter
            period_over  = 1;			// set the flag - 0.1s has passed
        }
    }
}


// Timer 2 counts the number of schmitt trigger edges in the input signal during each 0.1s period.
void timer2 (void) interrupt 5
{
	if (period_over)
    {
        frequency = schmitt_count * 10;	// calculate frequency in Hz, based on period count and interrupt period
        schmitt_count = 0;	            // reset the schmitt count for the next 0.1s period
        period_over = 0;                // reset the flag to start the next 0.1s period
    }
    else
    {
        schmitt_count++;                // increment the schmitt edge count for this period
    }
    TF2 = 0;					        // clear interrupt flag
}	// end timer2 interrupt service routine


void setup_frequency_timers()
{
    period_over = 0;		            // initialize the flag to 0
    period_count = 0;		            // initialize the interrupt counter to 0
    schmitt_count = 0;                  // initialize the schmitt edge count to 0
    frequency = 0;		                // initialize the frequency variable to 0

    // Set up the timer 0 interrupt (Taken from blinky-timer-2.c)
    TH0   = (unsigned char) PERIOD;		// set timer period
    TL0   = (unsigned char) PERIOD;
    TMOD |= 0x02;		                // select mode 2
    TR0   = 1;			                // start timer 0
    ET0   = 1;			                // enable timer 0 interrupt
    EA    = 1;			                // global interrupt enable

    // Set up timer 2 in timer mode, auto reload, and external control (Taken from FreqGen_timer2-2.c)
    T2CON = 0x06;                       // all zero except run control
    ET2   = 1;                          // enable timer 2 interrupt
    RCAP2 = 0xFFFF;                     // Set reload value to maximum, so it will overflow on every schmitt trigger edge
}


uint16 get_frequency_value()
{
    return frequency;
}
