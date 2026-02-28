/*  This program demonstrates how to use interrupts and a hardware timer.
	It uses a bit variable as a flag, to allow the interrupt service routine
	and the foreground program to communicate in a safe way.   */

#include <ADUC841.H>
#include "typedef.h"
#include "display.h"
#include "frequency_mode.h"
#include "adc_mode.h"

void main (void)
{
	uint8 mode;
	uint16 value;

	// Setup adc and display settings before going into the main loop
    adc_setup();
	display_setup();
	setup_frequency_timers();

	// After setting up, main goes into an infinite loop
	while (1)
	{
		mode = P2 & 0x07; // Read the 3 switch bits (P2.0, P2.1, P2.2)

		switch(mode)
		{
			case ADC_MODE: // First switch on, read ADC value in mV mode
				value = get_adc_mv_value();
				break;

			case FREQ_MODE: // Second switch on, read frequency value in Hz
				value = get_frequency_value();
				break;

			case AMP_MODE: // Third switch on, read amplitude value
				value = get_amplitude_value();
				break;

			default: // All switches off, or more than one switch on, display 0
				value = 0;
		}

		display(value, mode);
	}
}
