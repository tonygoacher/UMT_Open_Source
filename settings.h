#ifndef __SETTINGS_H
#define __SETTINGS_H



typedef enum
{
	eReserved0,
	eReserved1,
	eReserved2,
	eTeamId,
	eReserved3,
	eClipsFromAmmoBox,
	eHealthFromMedicBox,
	eReserved4,
	eHitLedTimeout,
	eSoundSet,
	eOverheatRoundsMin,
	eReserved5,
	eReserved6,
	eDamagePerShot,
	eClipSize,
	eNumberOfClips,
	eFireSelector,
	eBurstModeRoundCount,
	eCyclicRPM,
	eReloadDelay,
	eIrPower,
	eIrRange,
	eTagOnOffSettings,
	eRespawnHealth,
	eReserved7,
	eRespawnDelayTenthsSec,
	eArmourValue,
	eGameOnOffSettings1,
	eGameOnOffSettings2,
	eHitDelay,
	eStartDelay,
	eDeathDelay,
	eTimeLimit,
	eMaxRespawns,
	eReserved8,
	eNumDataItems,
}ECloneDataData;

typedef enum
{
	eGunId = eNumDataItems,
	eStartHealth,

}ESystemData;

// Settings team
typedef enum
{
	eTeamRed,
	eTeamBlue,
	eTeamYellow,
	eTeamGreen,
	eNumberOfTeams,
}ETeamId;

typedef enum
{
	eMilSim,
	eSciFi,
	eNumberOfSoundSets,
}ESoundSet;

typedef enum
{
	eDamage1,
	eDamage2,
	eDamage4,
	eDamage5,
	eDamage7,
	eDamage10,
	eDamage15,
	eDamage17,
	eDamage20,
	eDamage25,
	eDamage30,
	eDamage35,
	eDamage40,
	eDamage50,
	eDamage75,
	eDamage100,
	eNumberOfDamageValues,
}EDamagePerShot;

typedef enum
{
	eUnused = 0x01,
	eHitLedEnabled = 0x02,// Hit LED enabled
	eFriendlyFire = 0x04,// Friendly re enabled
	eUnlimitedClipsEnabled = 0x08, // Unlimited clips enabled
	eZombieMode = 0x10 , //Zombie mode enabled
	eMedicsEnabled = 0x20,// Medics enabled
	eGameBoxesREsetOnRespawn = 0x40, // Game boxes reset on respawn
	eGameBoxesNotUsedUp = 0x80, //Game boxes are not used up

}EGameSettings1;

typedef enum
{
	//0x01
//0x02
	eCaptureTheFlag = 0x04,// Capture-the-ag display enabled
	eRespawnEnabled = 0x08, // Respawn enabled
	eTaggerNicknamesEnabled = 0x10,// Tagger nicknames enabled
	eOldIrLevelField = 0x20,// Old IR level eld
	eFullAmmoResetOnRespawn = 0x40,// Full ammo reset on respawn
	eEnableGameMode = 0x80,// Enable game mode
}EGameSettings2;

typedef enum
{
	eUnlimited = 0xff,
}ENumberOfClips;

typedef enum
{
	eSemiAuto,
	eBurst,
	eFullAuto,
	eInfiniteBurst = 0xff,	// Internal, not part of protocol
	eNumFireSelectors,
}EFireSelector;






bit CheckCloneData();
void WriteCloneData();

#define GetTeamId() eeprom_read(eTeamId)

#define GetClipSize() eeprom_read(eClipSize)


#define GetNumberOfClips() eeprom_read(eNumberOfClips)



#define GetDamagePerShot() eeprom_read(eDamagePerShot)
#define GetStartHealth() eeprom_read(eStartHealth)
#define GetGunId() eeprom_read(eGunId)
#define GetDamagePerShot() eeprom_read(eDamagePerShot)
#define GetClipSize() eeprom_read(eClipSize)
#define GetNumberOfClips() eeprom_read(eNumberOfClips)
byte GetHitDelay();

#define GetStartHealth() eeprom_read(eStartHealth)

#define GetHitLedTimeout() eeprom_read(eHitLedTimeout)
#define GetRespawnHealth() eeprom_read(eRespawnHealth)
#define GetMaxRespawns() eeprom_read(eMaxRespawns)
#define GetDeathDelay() eeprom_read(eDeathDelay)
#define GetBurstModeRoundCount() eeprom_read(eBurstModeRoundCount)
#define GetFireSelector() eeprom_read(eFireSelector)
#define GetReloadTime() (eeprom_read(eReloadDelay)& 0x3f)
#define GetFriendlyFire() ((eeprom_read(eGameOnOffSettings1) & eFriendlyFire) !=0)

byte GetSystemBurstRoundCount();
void ProgramRoundsPerMin();

byte GetShotPower(byte byIndex);

#endif //__SETTINGS_H