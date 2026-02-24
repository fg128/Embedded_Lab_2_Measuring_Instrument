#include "display.h"

//writes 1byte of info to a reg
void write_spi(uint8 reg, uint8 data)
{
    //get the 16bits to be sent
    uint8 datah = reg & 0xF;
    uint8 datal = data;

    //pull line low
    P2 &= 0x01; //pull P2.1 low TMP not 100% which pin should be used

    //send data
    SPIDAT= datah; //send 1st byte
    //ideally this would be counter to stop a forever loop from occuring
    while((SPICON & 0x80) != 0x80);//wait for transfer to finish
    SPIDAT = datal;//send 2nd byte
    while((SPICON & 0x80) != 0x80);//wait for transfer to finish

    //pull line high
    P2 |= 0x01; //pull P2.1 high TMP not 100% which pin should be used
}

void init_display(void)
{
    //Need to set bits in the SFR register
    //set SPE 1, enables SPI
    //set SPIM 1, SCLK as output (Master mode)
    //set CPOL 0, CLK ideals low
    //set CPHA 1, use lead clk edge ? this allows the SS line to be pulled permentally low
    //set SPR0 to 1 to have bit rate ve f_osc/4

    //Set SPI configuration
    SPICON = 0x35;

    //set no decode mode
    write_spi(DEC_REG, 0x00);

    //set intensity
    write_spi(INT_REG, 0x0A); //like half intisity

    //set digits 0 1 2 3 4 5 ON
    write_spi(SCAN_REG, 0x05);

    //write IPSUM
    write_spi(DIG_0, LETTER_I);
    write_spi(DIG_1, LETTER_P);
    write_spi(DIG_2, LETTER_S);
    write_spi(DIG_3, LETTER_U);
    write_spi(DIG_4, LETTER_M_1);
    write_spi(DIG_5, LETTER_M_2);

    //enable the display
    write_spi(SHD_REG, 0x01);

}
