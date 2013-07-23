#ifndef __SPI_H_
#define __SPI_H_

#include <htc.h>
#include "types.h"



void SPIOut(byte data);
void SPIOutMSB(byte data);
void SPIInit();

void DisplaySSHigh();
void DisplaySSLow();
void ClockHigh();
void ClockLow();

#define DISPLAY_SS	RC1
#define	SS		RC4
#define MOSI	RC0
#define SCLK	RC3

#define ClockHigh() {SCLK=1;}

#define ClockLow() {SCLK=0;}

#define DisplaySSHigh(){DISPLAY_SS = 1;}

#define DisplaySSLow(){DISPLAY_SS = 0;}

#endif //__SPI_H_

