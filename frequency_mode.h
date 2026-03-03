#ifndef FREQUENCY_MODE_H
#define FREQUENCY_MODE_H

#include "typedef.h"

typedef enum{
	DC_MODE 	= 0x01,
	FREQ_MODE = 0x02,
	AMP_MODE 	= 0x04,
} MODE;

void setup_frequency_timers();

uint16 get_frequency_value();
uint16 get_amplitiude_value();
uint16 get_mDC_value();

#endif
