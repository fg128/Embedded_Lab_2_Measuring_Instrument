#ifndef DISPLAY_H
#define DISPLAY_H

#include "typedef.h"
#include <ADUC841.H>

//REGISTERS
//Digit Resigsters
#define DIG_0	0x01
#define DIG_1	0x02
#define DIG_2	0x03
#define DIG_3	0x04
#define DIG_4	0x05
#define DIG_5	0x06
#define DIG_6	0x07
#define DIG_7	0x08

//Controll Registers
#define DEC_REG  0x09 // Decode Mode Register
#define INT_REG  0x0A // Intensity Register
#define SCAN_REG 0x0B // Scan Limit Register
#define SHD_REG  0x0C // Shitdown Register


// 7-SEG definations
// Letters
#define LETTER_I 0x06
#define LETTER_P 0x67
#define LETTER_S 0x5B
#define LETTER_U 0x1C
#define LETTER_M_1 0x76
#define LETTER_M_2 0x70

// Numbers
#define NUM_1 0x30
#define NUM_2 0x6D
#define NUM_3 0x79
#define NUM_4 0x33
#define NUM_5 0x5B
#define NUM_6 0x5F
#define NUM_7 0x70
#define NUM_8 0x7F
#define NUM_9 0x73
#define NUM_0 0x7E

#define NUM_dot 0x80

// Start spi and initalizes the display
void init_display();
void write_spi(uint8 reg, uint8 data_to_write);


#endif
