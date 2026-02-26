#include "display.h"


sbit  LOAD = 0xB1; // TXD P3.0

void display_setup()
{
    // Need to set bits in the SFR register
    // Set SPE 1, enables SPI
    // Set SPIM 1, SCLK as output (Master mode)
    // Set CPOL 0, CLK ideals low
    // Set CPHA 0, use trail clk edge
    // Set SPR0 to 1 to have bit rate ve f_osc/4
	uint8 digits[8] = {6, 7, 8, 9, 0, 1, 2, 4};

    // Set SPI configuration
	SPE = 1;
	SPIM = 1;
	CPOL = 0;
	CPHA = 0;
	SPR0 = 1;

	// Set load (P3.0) to 1 so is high from start instead of floating
	LOAD = 1;

    // Set no decode mode
    write_spi(DEC_REG, 0x00);

    // Set intensity to about half
    write_spi(INT_REG, 0x0A);

    // Set digits 0 1 2 3 4 5 6 7 ON
    write_spi(SCAN_REG, 0x07);
}

// Writes 1 byte of info to a reg
void write_spi(uint8 address, uint8 data_to_write)
{
	uint8 i;
    address = address & 0x0F; 	  // Get 4 address bits for

    // Set load (P3.0) to 0
	LOAD = 0;

    // First send address to select a segment
    SPIDAT = address; 			  // Send address byte
	while(!ISPI);     			  // Wait for ISPI to be 1 to indcate finished
	for(i = 0; i < 10; i++); 	  // Small delay
	ISPI=0;

	// Then send data byte to indicate what to display on segmemt
    SPIDAT = data_to_write;   	  // Send data byte
	while(!ISPI);     			  // Wait for ISPI to be 1 to indcate finished
	ISPI = 0;

	// Set load (P3.0) to 1
	LOAD = 1;
}

// Writes the appropriate units to the display depending on the mode
void write_mode_units(uint8 mode)
{
	switch(mode)
	{
		case ADC_MODE:
			write_spi(DIG_5, LETTER_M_1);
			write_spi(DIG_6, LETTER_M_2);
			write_spi(DIG_7, LETTER_V);
			break;

		case HERTZ_MODE:
			write_spi(DIG_7, LETTER_H);
			break;

		case AMPLITUDE_MODE:
			write_spi(DIG_5, LETTER_V);
			write_spi(DIG_6, LETTER_P);
			write_spi(DIG_7, LETTER_P);
			break;

		default: // All switches off, or more than one switch on, display 0
			write_spi(DIG_5, 0x00);
			write_spi(DIG_6, 0x00);
			write_spi(DIG_7, 0x00);
	}
}

// Displays the value on the 8 digit 7-segment display
void display(uint16 value, uint8 mode)
{
	uint8 i, digit, segment;
	uint8 segments[11] = {NUM_0, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9, NUM_dot};
	uint8 dot_pos = 5;

	for(i = 0; i < 5; i++)
	{
		// Extract the least significant digit
		digit = value % 10;
		value /= 10;

		// Get the segment pattern for the digit, and add the dot if needed
		segment = segments[digit];
		if(i == dot_pos) segment |= 0x80;

		// Write the segment pattern to the corresponding display segment
		write_spi(i+1, segment);
	}

	// Depending on the mode write the appropriate units
	write_mode_units(mode);

	// Enable the display
    write_spi(SHD_REG, 0x01);
}


