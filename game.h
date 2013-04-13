#ifndef __GAME_H
#define __GAME_H

typedef enum
{
	eGameStatePowerUp,
	eGameStateOff,	// Game is not active, awaiting game start
	eGameStateInit,
	eGameStateOn,	// Game is on
	eGameStateReload,
	eGameStateHit,
	eGameStateRespawning,	// Respawn countdown
	eGameStateRespawn,	// Perform immediate respawn
	eGameStateDead,	
	eGameStateGetMedic,	// Awating medic. Respawn active after this state
	eGameStateAdminKill, // Dead followed by game over
	eGameStateGameOver,
}EGameState;

#define HITLED RC2


typedef struct
{
	EGameState	eGameState;
	unsigned int	iRounds;
	byte	byRoundsThisClip;
	byte	byHealth;
	byte 	byDeathTimer;
	byte 	byBurstCount;
	byte	byHitDelay;
	byte 	byHitLedTimeout;
	byte	byRespawns;
}SGameStatus;

void InitAmmo();


EGameState InitGame();
EGameState Powerup();
EGameState ProcessShot();




EGameState ProcessSystemMessage();


EGameState Hit();
EGameState Dead();
EGameState GameOver();
EGameState Game();
EGameState AdminKill();
EGameState Reload();

extern bank1 SGameStatus sGameState;


#endif //__GAME_H