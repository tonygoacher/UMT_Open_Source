#ifndef __SPI_H_
#define __SPI_H_

#include <htc.h>
#include "types.h"



void SPIOut(byte data);
void SPIOutMSB(byte data);
void SPIInit();
void SSLow();
void SSHigh();
void DisplaySSHigh();
void DisplaySSLow();
void ClockHigh();
void ClockLow();

#endif //__SPI_H_

