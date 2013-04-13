
#include <htc.h>
#include "led_display.h"
#include "spi.h"
#include "game.h"
#include "settings.h"
#include "interrupt.h"
#include "smalldivide.h"

static enum
{
	eResetDisplay = 0x76,
	eBlank = 0x78,
	eDigit1 = 0x7b,
	eDigit2 = 0x7c,
	eDigit3 = 0x7d,
	eDigit4 = 0x7e,
	eSetBrightness = 0x7a,
}EDisplayCommands;





static void SetBrightness()
{
	ClockLow();
	DisplaySSLow();
	SPIOutMSB(eResetDisplay);
	DisplaySSHigh();
	DisplaySSLow();
	SPIOutMSB(eSetBrightness);
	SPIOutMSB(0x80);			// Set max brightness
	DisplaySSHigh();
}


static bank1 byte byIndex;
static bank1 int iNumber;
static bank1 int iLocalDivisor;



static void DisplayNumber()
{
	ClockLow();
	DisplaySSLow();
	iLocalDivisor = 1000;
	for(byIndex = 0 ; byIndex < 4 ; byIndex++)
	{
		iDividend = iNumber;
		iDivisor = iLocalDivisor;
		SmallDivide();		
		iNumber = iDividend;
		// If the MS digit is 0 then blank the first segment of the display
		if(!iQuotient && !byIndex)
		{
			SPIOutMSB(eBlank);
		}
		else
		{
			SPIOutMSB(iQuotient | 0x30);
		}
		iDividend = iLocalDivisor;
		iDivisor = 10;
		SmallDivide();
		iLocalDivisor = iQuotient;
	}
	DisplaySSHigh();
}

// Display how much health
void HealthDisplay()
{
	iNumber = sGameState.byHealth;
	DisplayNumber();	
}

// Display how many rounds in this clip
void AmmoDisplay()
{
	iNumber = sGameState.byRoundsThisClip;
	DisplayNumber();
}

// Display hoiw many clips
void ClipDisplay()
{
	if(GetNumberOfClips() == eUnlimited)
	{
		iNumber = 999;
	}
	else
	{
		
		iNumber = sGameState.iRounds / GetClipSize();
	}
	DisplayNumber();
}


// byValue should have required data in it
static void DataToAll4Digits(byte byData)
{
	ClockLow();
	DisplaySSLow();
	ClockLow();
	for(byIndex = 0 ;byIndex < 4 ; byIndex++)
	{
		SPIOutMSB(eDigit1 + byIndex);
		SPIOutMSB(byData);
	}
	DisplaySSHigh();
}

void DeadDisplay()
{	
	DataToAll4Digits(0x40);	
}

void GameOverDisplay()
{
	DataToAll4Digits(0x00);
}

void InitDisplay()
{
	DisplaySSLow();
	DisplaySSLow();
	
	SetBrightness();
	DisplaySSHigh();
}


void ShowDeathTime()
{
	DisplaySSLow();
	iNumber = GetDelayTimer() >> 2;
	DisplayNumber();
	DisplaySSHigh();
}