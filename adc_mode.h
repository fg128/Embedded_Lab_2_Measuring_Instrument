#ifndef ADC_MODE_H
#define ADC_MODE_H

#include "typedef.h"
#include <ADUC841.H>

void adc_calibrate();
void adc_setup();
uint16 get_adc_mv_value();
