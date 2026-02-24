#include "typedef.h"
#include <ADUC841.H>
#include "display.h"

#define PERIOD        -250        // 250 clock cycles interrupt period
/* This negative value will be cast as an unsigned 8-bit integer below, and
   this will result in a timer reload value of 6.  Counting up from that value,
   the 8-bit timer will overflow after 250 clock cycles.  At a clock frequency
   of 11.0592 MHz, that gives interrupt frequency 44.2368 kHz, period ~22.6 us.  */

#define NUM_INTS    11059        // number of interrupts between events
// This value gives event frequency almost exactly 4 Hz, period 0.25 s


// These are global variables: static and available to all functions
unsigned int       TimerTick;       // global variable to count interrupts
bit                TimeOver;        // global variable - flag to signal event

typedef struct {
    uint32 value;
    uint16 offset;
    uint16 gain;
    uint16 mV;
} ADC;

ADC adc;

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

void adc_calibrate()
{
    ADCCON3 = 0x01; // Calibrate offset
    while((ADCCON3 & 0x01) == 0x01); // Wait until calibration is done
    adc.offset = ((ADCOFSH & 0x0F) << 8) | ADCOFSH;

    ADCCON3 = 0x03; // Calibrate gain
    while((ADCCON3 & 0x01) == 0x01); // Wait until calibration is done
    adc.gain = ((ADCGAINH & 0x0F) << 8) | ADCGAINL;
}

void adc_setup()
{
    ADCCON1 = 0xB0;
    ADCCON2 = 0x00; // Set to channel 0 output
    adc_calibrate();
}

uint16 get_adc_value()
{
      uint16 val;
    ADCCON2 |= 0x10; // Do one conversion
    while((ADCCON2 & 0x10) == 0x10); // Wait until ADC is done
        val = ((ADCDATAH & 0x0F) << 8) | ADCDATAL;
        return val;
}

//MAIN
void main (void)
{
    // Set up the timer 0 interrupt
    TH0   = (unsigned char) PERIOD;        // set timer period
    TL0   = (unsigned char) PERIOD;
    TMOD |= 0x02;        // select mode 2
    TR0   = 1;            // start timer 0
    ET0   = 1;            // enable timer 0 interrupt
    EA    = 1;            // global interrupt enable

    //Set up ADC
    adc_setup();

    //set up display
    init_display();

    // main loop
    while (1)
    {
        while(!TimeOver);        // wait until interrupt service routine sets flag
        TimeOver = 0;            // reset the flag

        adc.value = get_adc_value();
        adc.mV =  adc.value*2500L  / 4096L;
    }

}
