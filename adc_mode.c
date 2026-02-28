#include <ADUC841.H>
#include "typedef.h"

typedef struct {
	uint32 value;
	uint16 offset;
	uint16 gain;
	uint32 mV;
} ADC;

ADC adc;


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
