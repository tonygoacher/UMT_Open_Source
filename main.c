
#include <htc.h>
#include "types.h"
#include "interrupt.h"
#include "protocol.h"
#include "status.h"
#include "messageproc.h"
#include "settings.h"

#include "isd.h"
#include "game.h"

//#include "led_display.h"

#pragma switch space




void GlobalInit(void)
{	
	OSCCON=0x70;	// 8MHz

	// PWM setup
	PR2 = 0b00001000;
	CCPR1L = 0b00000100;	// PWM compare register
	T2CON  = 0b00000101;	

	ResetStatusByte();
	
	PSA=0;
	nRAPU=0;

	T0CS=0;			// Timer0 timer mode
	PS2=1;			// Timer0 /32 prescaler
	PS1=0;
	PS0=0;

	WPUA2=1;		//PORTA2 Weak pull up enabled
	WPUA1 = 1;		// Porta RA1 is reload
	WPUA0=1;		// Port RA0 is trigger
	CMCON0=0x07;	// Port C to digital IO
	ANSEL=0;
	SPIInit();
	TRISA=0xff;
	TRISC=0xc0;		// 11000000

	
// Init timer 1 for interrupt. Period is set in the interrupt itself
	TMR1IF = 0;
  	TMR1H	 = 0xFF;
  	TMR1L	 = 0xFF;	// Force immediate overflow so the actual value can be set
	TMR1IE = 1;
	TMR1ON = 1;
	PEIE =1;
	
	
	

	RxInterruptInit();
	InitIsd();
	GIE=1;
		
}


void main(void)
{
	GlobalInit();
		
	sGameState.eGameState = eGameStatePowerUp;

	for(;;)
	{
		switch(sGameState.eGameState)
		{
			case 	eGameStatePowerUp:
					{
						sGameState.eGameState = Powerup();
					}
					break;
			case	eGameStateOff:
					{

					}
					break;

			case	eGameStateInit:
					{
						sGameState.eGameState = InitGame();
					}	
					break;

			case	eGameStateOn:
					{
						sGameState.eGameState = Game();
					}
					break;	

			case	eGameStateRespawn:
					{

						InitGame();	// Reset all
						sGameState.byHealth = GetRespawnHealth(); // But set health to respawn value
						sGameState.eGameState = eGameStateOn;
					}
					break;

			case	eGameStateHit:
					{
						sGameState.eGameState = Hit();
					}
					break;

			case	eGameStateDead:
					{
						sGameState.eGameState = Dead();
					}
					break;
			case	eGameStateGameOver:
					{
						sGameState.eGameState = GameOver();
					}
					break;

			case	eGameStateAdminKill:
					{
						sGameState.eGameState = AdminKill();
					}
					break;

			case	eGameStateReload:
					{		
						sGameState.eGameState = Reload();
					}	
					break;
		}
	}


	
	
}