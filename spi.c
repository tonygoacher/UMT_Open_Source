#include <htc.h>

#include "spi.h"

#define MOSI	RC0
#define SCLK	RC3
#define	SS		RC4
#define DISPLAY_SS	RC1

byte bySPIBitCount;

void  SPIInit()
{
	SS=1;
	SCLK=1;
}


void SPIOut(byte data)
{

	bySPIBitCount = 8;
	while(bySPIBitCount--)
	{
		MOSI = data & 0x01;
		SCLK=0;
//		SCLK=0;
//		SCLK=1;
		SCLK=1;
		data >>=1;
		
	}
}

void SPIOutMSB(byte data)
{
	bySPIBitCount = 8;
	while(bySPIBitCount--)
	{
		if(data & 0x80)
		{
			MOSI = 1;
		}
		else
		{
			MOSI = 0;
		}
		SCLK=1;
		SCLK=0;
		data = data << 1;		
	}
}

void SSLow()
{
	SS=0;
}

void SSHigh()
{
	SS=1;
}

void ClockHigh()
{
	SCLK=1;
}

void ClockLow()
{
	SCLK=0;
}

void DisplaySSHigh()
{
	DISPLAY_SS = 1;
}

void DisplaySSLow()
{
	DISPLAY_SS = 0;
}