
#include "spi.h"


typedef enum
{
	eShotSound,
	eEmptySound,
	eClipSound,			// Pre reload
	eActionSound,		// Post reload
	eNearMissSound,
	eDamageSound,
	eDeadSound,
	ePowerUpSound,
	eBeepSound,
	eBuzzSound,
	eMedicSound,
	eGameOverSound,
	eCriticalSound,
	eBeep2Sound,
	eDisarmSound,	
} ESound;

typedef enum
{
	eIsdPowerUp = 0x01,
	eIsdStop = 0x02,
	eIsdClearInterrupts = 0x04,
	eIsdPowerDown = 0x07,
	eIsdWriteAPC2 = 0x65,
	eIsdPlay = 0x80,
	
}EIsdCommands;

// Audio control value sets FT  function disabled, Analogue ouput on aux
#define AUDCONTROLLOW  0b11000000	
#define AUDCONTROLHIGH 0b00001101
#define DEFAULTVOL  0b00000100 


void InitIsd();

void PlaySound(ESound eSound);	// Stack depth =2


