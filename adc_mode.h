#ifndef ADC_MODE_H
#define ADC_MODE_H

#include "typedef.h"
#include <ADUC841.H>

#define ADC_AVG 5 //number of adc values to average together

//struct to store adc values
// if we are not using calibration values this is useless
typedef struct {
	uint32 value;
	uint32 mV;
	uint16 offset;
	uint16 gain;
} ADC;


//functions
void adc_calibrate(); //calibrates the adc, gets offset and gain error
void adc_setup();			//sets up adc
uint16 get_adc_value(uint8 num_samples); //gets raw adc value

#endif
