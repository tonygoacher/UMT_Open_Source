

#include <htc.h>
#include "types.h"
#include "status.h"
#include "interrupt.h"
#include "settings.h"
#include "isd.h"
#include "protocol.h"
#include "game.h"
#include "led_display.h"

//#define MPLABSIM

#define TRIGGER 	RA0
#ifndef MPLABSIM
#define TRIGGER_PRESSED  (!TRIGGER)
#else
#define TRIGGER_PRESSED 	1
#endif

#define RELOAD		RA1
#ifndef MPLABSIM
#define RELOAD_PRESSED	(!RELOAD)
#else
#define RELOAD_PRESSED	0
#endif


#define DEFAULT_GAME_STATE  eGameStateOn  // The game state at power up
#pragma switch space

bank1 SGameStatus sGameState;

void SetHitLed()
{
	 HITLED = GetHitLedTimer() !=0;
}



// Process the system data command
// Check the data byte to find out what kind of system command it is
EEvent ProcessSystemData()
{
	if(rgbyDataBuffer[ePacketDataByte] ==eDataByteCloningData)
	{
		if(CheckCloneData())
		{
			WriteCloneData();
			
			// Wait for clone sound to finish
			SetDelayTimer(6);
			while(GetDelayTimer());
			return eEventCloneDataReceived;
		}
		else
		{
			return eEventNearMiss;
		}	
	}
	return eEventNearMiss;
}

// Process a command message
EEvent ProcessCommandMessage()
{
	// Check  data byte to find the system message type
	switch(rgbyDataBuffer[ePacketDataByte])
	{
		case	eCommandAdminKill:
				{
					return eEventAdminKill;
				}
		case	eCommandRespawn:
				{
					return eEventRespawn;
				}
		case	eCommandFullAmmo:
				{
					return eEventFullAmmo;
				}
		case	eCommandFullHealth:
				{
					return eEventFullHealth;
				}
		case	eCommandStartGame:
		case	eCommandImmediateNewGame:
				{
					return eEventStartGame;
				}
	}
	return eEventNullEvent;
}

// Priocess a system message command
// Need to check the command byte  to find out what
// kind of command it is
EEvent ProcessSystemMessage()
{
	switch(rgbyDataBuffer[ePacketHeader])
	{
		case  	eSystemData:
				{
					return ProcessSystemData();
				}			

		case	eCommand:
				{
					return ProcessCommandMessage();
				}
			
	}
	return eEventNullEvent;
}

void SetAmmo()
{
	sGameState.iRounds = GetNumberOfClips() * (GetClipSize() - 1);
	sGameState.byRoundsThisClip = GetClipSize();
}



// Start a game by resetting the ammo, health etc
EGameState InitGame()
{
	// Initialise ammo
	SetShotByte1(GetGunId());					// Configuure the shot packet
	SetShotByte2((GetTeamId() << 4) | GetDamagePerShot());

	SetAmmo();
	ProgramRoundsPerMin();
	
	if(GetFireSelector() == eSemiAuto)
	{
		sGameState.byBurstCount = 1;
	}
	if(GetFireSelector() == eBurst)
	{
		sGameState.byBurstCount = GetSystemBurstRoundCount();	
	}
	if(GetFireSelector() == eFullAuto)
	{
		sGameState.byBurstCount = eInfiniteBurst;
	}
	PlaySound(ePowerUpSound);
	sGameState.byHealth = GetStartHealth();
	return eGameStateOn;	
}

// Player may be returning to game
void ReturnToGame()
{
	HITLED = 0;
	SetDelayTimer(0);
	SetHitLedTimer(0);	// Reset the game timers
	sGameState.eGameState = DEFAULT_GAME_STATE;	// The game default state
	sGameState.byHitDelay = GetHitDelay();
	sGameState.byHitLedTimeout = GetHitLedTimeout() * 4;
	sGameState.byRespawns = GetMaxRespawns();
}

// This is called once to do a base level initialisation
// It may be called after receiving a configuration packet
EGameState Powerup()
{
	ReturnToGame();
	if(sGameState.eGameState == eGameStateOn)		// If the default state is on, go straight to init state
	{
		 	
		return eGameStateInit;
	}	
	return eGameStateOff;			// System is off, await new game packet
}











// Have been hit boy hostile or firendly fire if enabled
// but have enough health to continue the game.
// Do any sound effects or LED display here.
// Wait until 
EGameState Hit()
{
	HealthDisplay();
	PlaySound(eDamageSound);
	HITLED=1;
	SetHitLedTimer(sGameState.byHitLedTimeout);
	SetDelayTimer(sGameState.byHitDelay);
	while(GetDelayTimer())
	{
		SetHitLed();
	}
	ClearStatusBit(eDataReady);
	return eGameStateOn;
}


// Remain in the dead state until new game, medic or respawn
EGameState Dead()
{

	EEvent eEvent;
	HITLED = 1;
	DeadDisplay();
	PlaySound(eDeadSound);
	sGameState.byHealth = 0;
	// Now dead, wait for death delay and check for medic or respawn or new game packets
	SetDelayTimer(GetDeathDelay() <<4);
	while(GetDelayTimer())
	{
		ShowDeathTime();
		if(GetStatus() & eDataReady)
		{
			ClearStatusBit(eDataReady);
			if(rgbyDataBuffer[ePacketHeader] & SYSTEM_MESSAGE)
			{
					eEvent = ProcessSystemMessage();	// Get system message type
					if(eEvent == eEventRespawn)	// Respawn now if received
					{
						return eGameStateRespawn;		// Immediate respawn
					}

					if(eEvent == eEventStartGame)
					{
						return eGameStateInit;
					}

					if(eEvent == eEventEndGame)
					{
						return eGameStateGameOver;		// Immediate game over
					}

					if(eEvent == eEventCloneDataReceived)
					{
						PlaySound(eBeepSound);
						return eGameStatePowerUp;
					}

					if(eEvent == eEventNearMiss)
					{
						PlaySound(eNearMissSound);
						return sGameState.eGameState;
					}	


					if(eEvent == eEventFullHealth)
					{	
						sGameState.byHealth = GetStartHealth();
						ReturnToGame();
						PlaySound(ePowerUpSound);
						return eGameStateOn;

					}

					if(eEvent == eEventDisarmPlayer)
					{	
						sGameState.byRoundsThisClip = 0;
						sGameState.iRounds=0;
						PlaySound(eDisarmSound);
					}
			}
			
		}	
	}
	return eGameStateGameOver;
}

void ProcessShooting()
{
	if(TRIGGER_PRESSED)
	{


		if(!(GetStatus() & eShooting))
		{
			SetStatusBit(eShooting);	// Prevent further processing of new trigger event
			if(!sGameState.byRoundsThisClip)
			{
				PlaySound(eEmptySound);
				return;					// Clip empty, nothing else to be done
			}

			// Trigger press just detected, reset cyclic timer to be responsive
	
			ResetCyclicTimer();			//Ensure inter shot gap is consistent
			SetStatusBit(eCyclicTimer);	// Force immediate shot
		}

		if(GetStatus() & eCyclicTimer && sGameState.byBurstCount != 0 && sGameState.byRoundsThisClip != 0)
		{

			ClearStatusBit(eCyclicTimer);
			// Handle burst
			if(sGameState.byBurstCount != eInfiniteBurst)
			{
				sGameState.byBurstCount--;
			}
		
			
			Shoot();	// Start interrupt state machine
			sGameState.byRoundsThisClip--;
			PlaySound(eShotSound);
			AmmoDisplay();
			
		}		
	}
	else
	{
		// No trigger, but if previously shooting, reset burst count
		if(GetStatus() & eShooting)
		{
			ClearStatusBit(eShooting);
			sGameState.byBurstCount = GetSystemBurstRoundCount();
			__delay_ms(100);		// Debounce
		}
	}

}


EGameState ProcessShotPacket()
{

	byte byShotPower = GetShotPower(rgbyDataBuffer[ePacketShotData] & SHOT_POWER_BITS);
	// If same team, make sure it is not our own shot
	if(rgbyDataBuffer[ePacketShotData] >> 4 == GetTeamId() && rgbyDataBuffer[ePacketShotPlayer]== GetGunId())
	{
		return sGameState.eGameState;
	}

	// If a different team, or friendly fire is on then we are hit
	if((rgbyDataBuffer[ePacketShotData] >> 4 != GetTeamId()) || GetFriendlyFire())
	{
	
		if(sGameState.byHealth <= byShotPower)
		{
			return eGameStateDead;	
		}
		sGameState.byHealth -= byShotPower;
		return  eGameStateHit;
	}
	else
	{
		
		PlaySound(eNearMissSound);
	}
	return sGameState.eGameState;	// No state change
}

//////////////////////////////////////////////////////////////
// Process an in game IR packet and translate to a new state if required
/////////////////////////////////////////////////////////////
EGameState ProcessIrPacket()
{
	EEvent eEvent;
	ClearStatusBit(eDataReady);
	if(rgbyDataBuffer[ePacketHeader] & SYSTEM_MESSAGE)
	{
		eEvent =  ProcessSystemMessage();			// Get the event type
		switch(eEvent)
		{
			case	eEventCloneDataReceived:
			case 	eEventImmediateNewGame:
			{
					return eGameStatePowerUp;
			}

			case 	eEventNearMiss:
			{
					PlaySound(eNearMissSound);
			}
			break;

			case	eEventAdminKill:
			{
				return eGameStateAdminKill;
			}

			case	eEventEndGame:
			{
				return eGameStateGameOver;
			}

			case	eEventFullAmmo:
			{
				PlaySound(eBeepSound);
				SetAmmo();
			}

			case	eEventFullHealth:
			{
				PlaySound(eBeepSound);
				sGameState.byHealth = GetStartHealth();
			}

		};	
	

			
	}
	else
	{
		return ProcessShotPacket();
	}
	return sGameState.eGameState;	// No state change
}


/////////////////////////////////////////////
// The main game state SD:1
/////////////////////////////////////////////
EGameState Game()
{
	EGameState eNewState;
	AmmoDisplay();						// Alwways start by displaying the current clip contents
	for(;;)
	{	
		SetHitLed();					// Update the hit LED
		if(GetStatus() & eTimedOut)		// Play audio if protocol error
		{
			PlaySound(eNearMissSound);
			ClearStatusBit(eTimedOut);
		}
	
		ProcessShooting();				// Process the trigger/ammo etc.

		// Check reload
		if(RELOAD_PRESSED)				// See if we need to enter the reload state
		{
			return eGameStateReload;	
		}

		if(GetStatus() & eDataReady)		// Check if an IR packet was received in the background
		{
			eNewState = ProcessIrPacket();	// Got an IR packet, process it and get new state if any	
			if(eNewState != sGameState.eGameState)	// If a state change, return the new state to the state machine
			{
				return eNewState;
			}			
		}
	}
}

/////////////////////////////////////////////////////////////////
// The game over state
// Play game over sound and await new game
///////////////////////////////////////////////////////////////
EGameState GameOver()
{
	EEvent eEvent;
	GameOverDisplay();				// Change the display to game over mode
	PlaySound(eGameOverSound);		// Game over sound
	HITLED=1;						// Make sure the hit led is on
	for(;;)
	{
		if(GetStatus() & eDataReady)	// Wait for an IR packet
		{
			ClearStatusBit(eDataReady);	// Got an IR packet
			if(rgbyDataBuffer[ePacketHeader] & SYSTEM_MESSAGE)	// Only bother with system messages
			{
				eEvent = ProcessSystemMessage();
				if(eEvent == eEventStartGame || eEvent == eEventCloneDataReceived)	//Must only process a subset of messages in the state
				{
					return eGameStateInit;
				}	
			}
		}
	}
}


//////////////////////////////////////////////////////////////////
// Admin kill state is immediate death followed by game over
/////////////////////////////////////////////////////////////////
EGameState AdminKill()
{
	
	HITLED = 1;
	sGameState.byHealth = 0;
	PlaySound(eDeadSound);
	SetDelayTimer(8);	// Two second delay = 2s / 250mS = 8
	while(GetDelayTimer());
	return eGameStateGameOver;
		
}

////////////////////////////////////////////////////////
// The reload state
////////////////////////////////////////////////////////
EGameState Reload()
{
	EGameState eNewState;

	// If we have a full clkip, show the health until hit or no reload
	if(sGameState.byRoundsThisClip == GetClipSize())
	{
		HealthDisplay();						// Show the health value
		while(RELOAD_PRESSED)
		{
			if(GetStatus() & eDataReady)		// Still look for hits and system messages
			{
				eNewState = ProcessIrPacket();	// New state may be obtained
				if(eNewState != sGameState.eGameState)
				{
					AmmoDisplay();
					return eNewState;
				}			
			}
		}
		AmmoDisplay();						// Display ammo again
		return eGameStateOn;				// Continue with the game state
	}
	
	// If there are no rounds left, play the empty sound
	if(!sGameState.iRounds)
	{
		PlaySound(eEmptySound);
		// Wait here for reload to be released whilst processing messages
		while(RELOAD_PRESSED)
		{
			if(GetStatus() & eDataReady)		// Still look for hits and system messages
			{
				eNewState = ProcessIrPacket();	// New state may be obtained
				if(eNewState != sGameState.eGameState)
				{
					return eNewState;
				}			
			}
		}
	}
	else
	{
		// Rounds remaining so can reload
		PlaySound(eClipSound);					// Play the clip sound
		SetDelayTimer(GetReloadTime() << 2);	// Start a delay in seconds
	
		while(GetDelayTimer())
		{
			if(GetStatus() & eDataReady)		// Still look for hits and system messages
			{
				eNewState = ProcessIrPacket();
				if(eNewState != sGameState.eGameState)
				{
					return eNewState;			// New state required
				}			
			}
		}
		PlaySound(eActionSound);				// Play action sound to confirm reload
		// First put remaining clip contents back into total rounds
		sGameState.iRounds += sGameState.byRoundsThisClip;
		sGameState.byRoundsThisClip = 0;
		if(GetClipSize() > sGameState.iRounds)	// If there are not enough rounds to fill a clip, use
		{										// what is left
			sGameState.byRoundsThisClip = sGameState.iRounds;
			sGameState.iRounds = 0;
		}
		else
		{
			// At least enough rounds to fill a clip
			sGameState.byRoundsThisClip = GetClipSize();
			// Only subtract from rounds if not infinite
			if(GetNumberOfClips() != eUnlimited)
			{
				sGameState.iRounds -= GetClipSize();	
			}
		}
	}
	AmmoDisplay();
	
	return eGameStateOn;	
}
