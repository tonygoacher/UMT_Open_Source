

#include <htc.h>
#include "types.h"
#include "settings.h"
#include "interrupt.h"
#include "protocol.h"





// Define the clone packet structure
typedef struct
{
	byte byHeader;
	byte byCommand;
	byte byValidator;
	byte byReserved0;	// <<<<<<EEPROM storage starts here
	byte byReserved1;
	byte byReserved2;
	byte byTeamId;
	byte brReserved3;
	byte byClipsFromAmmoBox;
	byte byHealthFromMedicBox;
	byte byReserved4;
	byte byHitLedTimeout;
	byte bySoundSet;
	byte byOverheatRoundsMin;
	byte byReserved5;
	byte byReserved6;
	byte byDamagePerShot;
	byte byClipSize;
 	byte byNumberOfClips;
	byte byFireSelector;
	byte byBurstModeRoundCount;
	byte byCyclicRPM;
	byte byReloadDelay;
	byte byIrPower;
    byte byIrRange;
    byte byTagOnOffSettings;
	byte byRespawnHealth;
	byte byReserved7;
	byte byRespawnDelayTenthsSec;
	byte byArmourValue;
    byte byGameOnOffSettings1;
	byte byGameOnOffSettings2;
	byte byHitDelay;
    byte byStartDelay;
    byte byDeathDelay;
    byte byTimeLimit;
	byte byMaxRespawns;
	byte byReserved8;
	byte byChecksum;
}SClonePacket;




#define EE_DEFAULT_RESERVED0				0
#define EE_DEFAULT_RESERVED1				0
#define EE_DEFAULT_RESERVED2				0
// 0 = red, 1 = blue , 2 = yellow , 3 = green
#define	EE_DEFAULT_TEAM_ID					3	
#define EE_DEFAULT_RESERVED3				0
#define EE_DEFAULT_CLIPS_FROM_AMMO_BOX		5
#define	EE_DEFAULT_HEALTH_FROM_MEDIC_BOX	100	
#define EE_DEFAULT_RESERVED4	0

// Addr 0
__EEPROM_DATA(EE_DEFAULT_RESERVED0,\
EE_DEFAULT_RESERVED1,\
 EE_DEFAULT_RESERVED2, \
EE_DEFAULT_TEAM_ID,\
 EE_DEFAULT_RESERVED3,\
 EE_DEFAULT_CLIPS_FROM_AMMO_BOX, \
EE_DEFAULT_HEALTH_FROM_MEDIC_BOX,\
 EE_DEFAULT_RESERVED4);


#define EE_DEFAULT_HIT_LED_TIMEOUT	1
// 0 = milsim, 1 = scifi
#define EE_DEFAULT_SOUND_SET 		0
#define EE_DEFAULT_OVERHEAT_RPM		40
#define EE_DEFAULT_RESERVED5				0
#define EE_DEFAULT_RESERVED6				0
// See EDamagePerShot
#define EE_DEFAULT_DAMAGE_PER_SHOT			5
#define	EE_DEFAULT_CLIP_SIZE				30
// 0xff = unlimited
#define	EE_DEFAULT_NUMBER_OF_CLIPS			0xff

// Addr 8	
__EEPROM_DATA(EE_DEFAULT_HIT_LED_TIMEOUT,\
EE_DEFAULT_SOUND_SET,\
EE_DEFAULT_OVERHEAT_RPM,\
EE_DEFAULT_RESERVED5,\
EE_DEFAULT_RESERVED6,\
EE_DEFAULT_DAMAGE_PER_SHOT,\
EE_DEFAULT_CLIP_SIZE,\
EE_DEFAULT_NUMBER_OF_CLIPS);


// 0 = semi, 1 = burst, 2 = full
#define EE_DEFAULT_FIRE_SELECTOR	1
#define EE_DEFAULT_BURST_MODE_ROUND_COUNT	3
// 0 = 250rpm. See 
#define EE_DEFAULT_CYCLIC_RPM	1
// REload delay in sec
#define EE_DEFAULT_RELOAD_DELAY	4
// 0 = indoor, 1 = outdoor
#define EE_DEFAULT_IR_POWER		1
// 1 = min, 2 = 10%, 4 = 20%, 7 = 40% a = 60% 0x0e = 80% 0x12 - 100%
#define EE_DEFAULT_IR_RANGE		0x12
//  Logical or of:
// 1 = muzzle flash on
// 2 = Overheat feature on
#define EE_DEFAULT_TAG_ON_OFF_SETTINGS	0
#define EE_DEFAULT_RESPAWN_HEALTH	100

// Addr16
__EEPROM_DATA(EE_DEFAULT_FIRE_SELECTOR,\
EE_DEFAULT_BURST_MODE_ROUND_COUNT,\
EE_DEFAULT_CYCLIC_RPM,\
EE_DEFAULT_RELOAD_DELAY,\
EE_DEFAULT_IR_POWER,\
EE_DEFAULT_IR_RANGE,\
EE_DEFAULT_TAG_ON_OFF_SETTINGS,\
EE_DEFAULT_RESPAWN_HEALTH);

#define EE_DEFAULT_RESERVED7	0
#define EE_DEFAULT_RESPAWN_DELAY	1
#define EE_DEFAULT_ARMOUR_VALUE	0
// Logical or of the following:
//0x01
//0x02 Hit LED enabled
//0x04 Friendly re enabled
//0x08 Unlimited clips enabled
//0x10 Zombie mode enabled
//0x20 Medics enabled
//0x40 Game boxes reset on respawn
//0x80 Game boxes are not used up
#define EE_DEFAULT_GAME_SETTINGS1 0x02 
// Logical or of the following value
//0x01
//0x02
//0x04 Capture-the-ag display enabled
//0x08 Respawn enabled
//0x10 Tagger nicknames enabled
//0x20 Old IR level eld
//0x40 Full ammo reset on respawn
//0x80 Enable game mode
#define EE_DEFAULT_GAME_SETTINGS2	(0x40)
// Hit delay . 
#define	EE_DEFAULT_HIT_DELAY	2
#define EE_DEFAULT_START_DELAY	10
#define EE_DEFAULT_DEATH_DELAY	5

// Addr 24
__EEPROM_DATA(EE_DEFAULT_RESERVED7,\
EE_DEFAULT_RESPAWN_DELAY,\
EE_DEFAULT_ARMOUR_VALUE,\
EE_DEFAULT_GAME_SETTINGS1,\
EE_DEFAULT_GAME_SETTINGS2,\
EE_DEFAULT_HIT_DELAY,\
EE_DEFAULT_START_DELAY,\
EE_DEFAULT_DEATH_DELAY);


// No time limit
#define EE_DEFAULT_TIME_LIMIT	0	
#define	EE_DEFAULT_MAX_RESPAWNS		2
#define EE_DEFAULT_RESERVED8	0
#define EE_DEFAULT_GUN_ID		0
#define	EE_DEFAULT_HEALTH		100

// Addr 32
__EEPROM_DATA(EE_DEFAULT_TIME_LIMIT,\
EE_DEFAULT_MAX_RESPAWNS	,\
 EE_DEFAULT_RESERVED8,\
EE_DEFAULT_GUN_ID,\
EE_DEFAULT_HEALTH,\
0,0,0);

// Store shot power lookup in EEPROM to save ROM
//addr 40
#define EEPROM_SHOT_POWER_DATA 40
__EEPROM_DATA(1,2,4,5,7,10,15,17);
// Addr 48
__EEPROM_DATA(20,25,30,35,40,50,75,100);
// Addr 56
// Hit delay lookup 
#define EEPROM_HIT_DELAY_DATA  56
__EEPROM_DATA(0,1,2,3,4,8,12,16);
// addr 64
__EEPROM_DATA(20,24,28,32,36,40,44,48);
// addr 72
__EEPROM_DATA(52,56,60,64,68,72,76,80);




byte GetShotPower(byte byIndex)
{
	return eeprom_read(EEPROM_SHOT_POWER_DATA + byIndex);
}







typedef struct
{
	byte byAddress;
	byte byData;

}SSettingsData;

bank1 SSettingsData sSettingsData;


#define offsetof(st, m) ((int)(&((st *)0)->m))



// Verify the clone data checksum
// Returns true if data is valid
bit CheckCloneData()
{
	SClonePacket* psClonePacket;
	byte byChecksum=0;
	byte byIndex;
	psClonePacket = (SClonePacket*) rgbyDataBuffer;

	for(byIndex = offsetof(SClonePacket,byReserved0);	//3
		// Subtract one to remove the cehcksum value from the checksum calculation
		byIndex < offsetof(SClonePacket,byChecksum); //37
		byIndex++)
	{
		byChecksum += rgbyDataBuffer[byIndex];		
	}
	return psClonePacket->byChecksum == byChecksum;
}



void WriteCloneData()
{
	GIE=0;
	for(sSettingsData.byAddress = eReserved0 ; sSettingsData.byAddress < eNumDataItems ; sSettingsData.byAddress++)
	{
		sSettingsData.byData = rgbyDataBuffer[sSettingsData.byAddress + offsetof(SClonePacket,byReserved0)];	

// Inline write to save on stack
		EEADR = sSettingsData.byAddress;	// Set up data and address registers
		EEDAT = sSettingsData.byData;
		WREN=1;
		while(GIE);							// EEPROM write sequnce is a bit odd
		EECON2=0x55;
		EECON2=0xAA;
		WR=1;
		while(!EEIF);
		EEIF=0;
		WREN=0;

	}
	GIE=1;	

}






// Return hit delay in 250mS
byte GetHitDelay()
{

	return eeprom_read( eeprom_read(eHitDelay) + EEPROM_HIT_DELAY_DATA);
}


// Cyclic timer operates at 600uS period. so cyclic RPM uses this table
// to look up the delay
#define RPM_IN_600uS(rpm) (int)((100000/(rpm/6))/6)
static const int skrgiRoundsPerMinPeriod[]=
{
	RPM_IN_600uS(250),	
	RPM_IN_600uS(300),
	RPM_IN_600uS(350),
	RPM_IN_600uS(400),
	RPM_IN_600uS(450),
	RPM_IN_600uS(500),
	RPM_IN_600uS(550),
	RPM_IN_600uS(600),
	RPM_IN_600uS(650),
	RPM_IN_600uS(700),
	RPM_IN_600uS(750),
	RPM_IN_600uS(800),
};

void ProgramRoundsPerMin()
{
	byte byRounds = eeprom_read(eCyclicRPM);
	SetCyclicTimer(skrgiRoundsPerMinPeriod[byRounds]);
}

byte GetSystemBurstRoundCount()
{
	byte byFireSelector = GetFireSelector();
	switch(byFireSelector)
	{
		case	eSemiAuto:
				{
					return 1;		// One shot per burst
				}


		case	eBurst:
				{
					return GetBurstModeRoundCount();
				}
			
	}	
	return eInfiniteBurst;		// Infinite
}