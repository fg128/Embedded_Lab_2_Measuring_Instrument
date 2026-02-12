/*  This program demonstrates how to use interrupts and a hardware timer.
    It uses a bit variable as a flag, to allow the interrupt service routine
    and the foreground program to communicate in a safe way.   */

#include <ADUC841.H>
#include "typedef.h"

#define PERIOD        -250        // 250 clock cycles interrupt period
/* This negative value will be cast as an unsigned 8-bit integer below, and
   this will result in a timer reload value of 6.  Counting up from that value,
   the 8-bit timer will overflow after 250 clock cycles.  At a clock frequency
   of 11.0592 MHz, that gives interrupt frequency 44.2368 kHz, period ~22.6 us.  */

#define NUM_INTS    11059        // number of interrupts between events
// This value gives event frequency almost exactly 4 Hz, period 0.25 s

#define LED_BIT        0x10        // position of LED on port 3

// These are global variables: static and available to all functions
unsigned int    TimerTick;        // global variable to count interrupts
bit                TimeOver;        // global variable - flag to signal event


/*------------------------------------------------
Interrupt service routine for timer 0 interrupt.
"Called" by the hardware when the interrupt occurs.
------------------------------------------------*/
void timer0 (void) interrupt 1         // interrupt vector at 000BH
{
    TimerTick++;                    // increment interrupt counter
    if (TimerTick > NUM_INTS)         // if enough interrupts have been counted
    {
        TimerTick = 0;                // reset the counter
        TimeOver  = 1;                // set the flag - event has occurred
    }
}

void adc_setup()
{
    ADCCON1 = 0xB0;
    ADCCON2 = 0x00;
}

uint16 get_adc_sample()
{
    ADCCON2 |= 0x10;
    while((ADCCON2 & 0x10) == 0x10);
    return ((ADCDATAH & 0x0F) << 8) | ADCDATAL;
}

/*------------------------------------------------
The main C function.  Program execution starts
here after stack initialization.
------------------------------------------------*/
void main (void)
{
    // Set up the timer 0 interrupt
    uint16 sample;
    TH0   = (unsigned char) PERIOD;        // set timer period
    TL0   = (unsigned char) PERIOD;
    TMOD |= 0x02;        // select mode 2
    TR0   = 1;            // start timer 0
    ET0   = 1;            // enable timer 0 interrupt
    EA    = 1;            // global interrupt enable
    adc_setup();

    // After setting up, main goes into an infinite loop
    while (1)
    {
        while(!TimeOver);        // wait until interrupt service routine sets flag
        TimeOver = 0;            // reset the flag
        sample = get_adc_sample();
    }
/*  Note: The TimeOver flag is changed in main and in the ISR.  This is safe here:
    1: The operations should be atomic, as this is a bit variable
    2: Main only changes the variable just after the ISR has changed it, and
        the ISR will not change it again for many thousands of clock cycles.  */

}    // end of main
