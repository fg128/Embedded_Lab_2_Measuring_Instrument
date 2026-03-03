#include <ADUC841.H>
#include "typedef.h"
#include "adc_mode.h"

//global struct to store adc values
ADC adc;

//calibrates ADC
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
	//MD1 		= 1, 	power up the ADC
	//EXT_REF = 0, 	use internal reference
	//CK      = 11, ADC clk = master clk/2 ~5.5MHz
	//AQ      = 11, acqusistion of 4 ADC clks
	//T2C			= 0,  Not using timer 2 to start converstion
	//EXC			= 0,  Not using external trigger to start converstion
  ADCCON1 = 0xBC;
	//adc converstion time of, 5.5MHz/(16+4) = 275KHz

	adc_calibrate();
}


uint16 get_adc_value()
{
	uint32 avg = 0;
	uint8 i = 0;

	for(i = 0; i < ADC_AVG; i++)
	{
		ADCCON2 |= 0x10; // Trigger single conversion (Sets SCONV, Bit 4)

		// Wait for ADCI flag (Bit 7) to become 1, meaning conversion is done
		while((ADCCON2 & 0x10) == 0x10);
		// ADCCON2 &= ~0x80; // Manually clear the ADCI flag

		// Read the 12-bit value raw from the ADC data registers
		avg  += ((ADCDATAH & 0x0F) << 8) | ADCDATAL;
	}
	avg /= ADC_AVG; //mean the value

	return (uint16) avg;
}
