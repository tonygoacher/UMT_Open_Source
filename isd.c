
#include <htc.h>
#include "types.h"
#include "spi.h"
#include "isd.h"



#define ShotSoundAddressStart	17
#define	ShotSoundAddressEnd		34
#define	EmptySoundAddressStart	36
#define	EmptySoundAddressEnd	47
#define ClipSoundAddressStart	49
#define ClipSoundAddressEnd		60
#define ActionSoundAddressStart	62
#define ActionSoundAddressEnd	79
#define	NearMissSoundAddressStart	81
#define	NearMissSoundAddressEnd		98
#define	DamageSoundAddressStart	100
#define	DamageSoundAddressEnd	123
#define	DeadSoundAddressStart	125
#define	DeadSoundAddressEnd		160
#define	PowerUpSoundAddressStart	162
#define	PowerUpSoundAddressEnd	209
#define	BeepSoundAddressStart	211
#define	BeepSoundAddressEnd		222
#define	BuzzSoundAddressStart	224
#define	BuzzSoundAddressEnd		235
#define MedicSoundAddressStart	237
#define MedicSoundAddressEnd	248
#define	GameOverSoundAddressStart	0x100
#define	GameOverSoundAddressEnd	0x111
#define	CriticalSoundAddressStart	0x112
#define	CriticalSoundAddressEnd	0x123
#define	Beep2SoundAddressStart	0x126
#define	Beep2SoundAddressEnd	0x137
#define	DisarmSoundAddressStart	0x138
#define	DisarmSoundAddressEnd	0x149
#define LOWBYTE(value) (value & 0xff)
#define HIGHBYTE(value) (value / 0x100)



	

static const struct
{
	byte	byStartLowByte;
	byte	byStartHighByte;
	byte	byEndLowByte;
	byte	byEndHighByte;
}rgcSoundAddress[]=
{
	{LOWBYTE(ShotSoundAddressStart),	HIGHBYTE(ShotSoundAddressStart),	LOWBYTE(ShotSoundAddressEnd),		HIGHBYTE(ShotSoundAddressEnd) },
	{LOWBYTE(EmptySoundAddressStart),	HIGHBYTE(EmptySoundAddressStart),	LOWBYTE(EmptySoundAddressEnd),		HIGHBYTE(EmptySoundAddressEnd) },
	{LOWBYTE(ClipSoundAddressStart),	HIGHBYTE(ClipSoundAddressStart),	LOWBYTE(ClipSoundAddressEnd),		HIGHBYTE(ClipSoundAddressEnd) },
	{LOWBYTE(ActionSoundAddressStart),	HIGHBYTE(ActionSoundAddressStart),	LOWBYTE(ActionSoundAddressEnd),		HIGHBYTE(ActionSoundAddressEnd) },
	{LOWBYTE(NearMissSoundAddressStart),HIGHBYTE(NearMissSoundAddressStart),LOWBYTE(NearMissSoundAddressEnd),	HIGHBYTE(NearMissSoundAddressEnd) },
	{LOWBYTE(DamageSoundAddressStart),	HIGHBYTE(DamageSoundAddressStart),	LOWBYTE(DamageSoundAddressEnd),		HIGHBYTE(DamageSoundAddressEnd) },
	{LOWBYTE(DeadSoundAddressStart),	HIGHBYTE(DeadSoundAddressStart),	LOWBYTE(DeadSoundAddressEnd),		HIGHBYTE(DeadSoundAddressEnd) },
	{LOWBYTE(PowerUpSoundAddressStart),	HIGHBYTE(PowerUpSoundAddressStart),	LOWBYTE(PowerUpSoundAddressEnd),	HIGHBYTE(PowerUpSoundAddressEnd) },
	{LOWBYTE(BeepSoundAddressStart),	HIGHBYTE(BeepSoundAddressStart),	LOWBYTE(BeepSoundAddressEnd),		HIGHBYTE(BeepSoundAddressEnd) },
	{LOWBYTE(BuzzSoundAddressStart),	HIGHBYTE(BuzzSoundAddressStart),	LOWBYTE(BuzzSoundAddressEnd),		HIGHBYTE(BuzzSoundAddressEnd) },
	{LOWBYTE(MedicSoundAddressStart),	HIGHBYTE(MedicSoundAddressStart),	LOWBYTE(MedicSoundAddressEnd),		HIGHBYTE(MedicSoundAddressEnd) },
	{LOWBYTE(GameOverSoundAddressStart),HIGHBYTE(GameOverSoundAddressStart),LOWBYTE(GameOverSoundAddressEnd),	HIGHBYTE(GameOverSoundAddressEnd) },
	{LOWBYTE(CriticalSoundAddressStart),HIGHBYTE(CriticalSoundAddressStart),LOWBYTE(CriticalSoundAddressEnd),	HIGHBYTE(CriticalSoundAddressEnd) },
	{LOWBYTE(Beep2SoundAddressStart),	HIGHBYTE(Beep2SoundAddressStart),	LOWBYTE(Beep2SoundAddressEnd),		HIGHBYTE(Beep2SoundAddressEnd) },	
	{LOWBYTE(DisarmSoundAddressStart),	HIGHBYTE(DisarmSoundAddressStart),	LOWBYTE(DisarmSoundAddressEnd),		HIGHBYTE(DisarmSoundAddressEnd) },
};






void Stop()
{
	SSLow();
	SPIOut(eIsdStop);
	SPIOut(0x00);
	SSHigh();
}


void InitIsd()
{
	SSLow();
	SPIOut(eIsdPowerUp);
	SPIOut(0x00);
	SSHigh();
	SSLow();
	SPIOut(eIsdWriteAPC2);
	SPIOut(AUDCONTROLLOW | DEFAULTVOL);
	SPIOut(AUDCONTROLHIGH);
	SSHigh();
}


void PlaySound(ESound eSound)
{
	ClockHigh();	
	Stop();
	__delay_ms(30);
	SSLow();
	ClockHigh();
	SPIOut(eIsdPlay);
	SPIOut(0);		// Data byte 1 of play command is always 0
	SPIOut(rgcSoundAddress[eSound].byStartLowByte);
	SPIOut(rgcSoundAddress[eSound].byStartHighByte);
	SPIOut(rgcSoundAddress[eSound].byEndLowByte);
	SPIOut(rgcSoundAddress[eSound].byEndHighByte);
	SPIOut(0);		// Reserved address is set to 0
	SSHigh();		
}