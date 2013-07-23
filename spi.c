#include <htc.h>

#include "spi.h"





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





