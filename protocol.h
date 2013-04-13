#ifndef __PROTOCOL_H
#define __PROTOCOL_H


typedef struct
{
	byte	playerId;
	byte	shotPower;
}SShotPacket;

#define SHOT_POWER_BITS	0x0F
#define SHOT_TEAM_BITS	0x30

typedef struct
{
	byte header;
	byte dataValue;
}SMessagePacket;

typedef struct
{
	union
	{	
		SShotPacket shotPacket;
		SMessagePacket	messagePacket;
	};
}SMessage;

enum 
{
	ePacketHeader ,	// Offset into data buffer for packet header
	ePacketDataByte,
	ePacketTerminatorByte, 
	eSizePacketHeader,
};

// The state machine needs events, and because the different system and command bytes
// are all over the place, I replace them with a common set of events that can
// be passed back as events to an individual state
typedef enum
{
	eEventNullEvent,
	eEventAddHealth,
	eEventAddRounds,
	eEventClipsPickup,
	eEventHealthPickup,
	eEventFlagPickup,
	eEventAdminKill,
	eEventPauseUnpause,
	eEventStartGame,
	eEventRestoreDefaults,
	eEventRespawn,
	eEventImmediateNewGame,
	eEventFullAmmo,
	eEventEndGame,
	eEventResetClock,
	eEventInitialisePlayer,
	eEventExplodePlayer,
	eEventNewGameReady,
	eEventFullHealth,
	eEventFullArmour,
	eEventClearScores,
	eEventTestSensorLeds,
	eEventStunPlayer,
	eEventDisarmPlayer,
	eEventCloneDataReceived,
	eEventNearMiss,
	
}EEvent;


//Commmand byte values
typedef enum
{
	eAddHealth = 0x80,
	eAddRounds,
	eCommandByteReserved1,
	eCommand,
	eCommandByteReserved2,
	eCommandByteReserved3,
	eCommandByteReserved4,
	eSystemData,
	eCommandByteReserved5,
	eCommandByteReserved6,
	eClipsPickup,
	eHealthPickup,
	eFlagPickup,
}ECommandByte;

// Define command data byte values
typedef enum
{
	eCommandAdminKill,
	eCommandPauseUnpause,
	eCommandStartGame,
	eCommandRestoreDefaults,
	eCommandRespawn,
	eCommandImmediateNewGame,
	eCommandFullAmmo,
	eCommandEndGame,
	eCommandResetClock,
	eCommandReserved0,
	eCommandInitialisePlayer,
	eCommandExplodePlayer,
	eCommandNewGameReady,
	eCommandFullHealth,
	eCommandReserved1,
	eCommandFullArmour,
	eCommandReserved2,
	eCommandReserved3,
	eCommandReserved4,
	eCommandReserved5,
	eCommandClearScores,
	eCommandTestSensorLeds,
	eCommandStunPlayer,
	eCommandDisarmPlayer,

}ECommands;

// Data byte values
typedef enum
{
	eDataByteReserved1,
	eDataByteCloningData,
	eDataByteReserved2,
	eDataByteScorePart1,
	eDataByteScorePart2,
	eDataByteScorePart3,
}EDataByte;

typedef enum
{
	eHeader,
	eDataByte,
	eMessageTerminator,
	eSystemMessage,
	eSystemMessageTerminator,
	eCloneMessageTerminator,
	eStateCloneData2,
	eComplete,

}EProtocolState;



extern EProtocolState eProtocolState;

#define ResetProtocol() (eState = eHeader)




#define SYSTEM_MESSAGE	0x80

#define PACKET_HEADER	0


extern byte ProcessMessage(byte eData);




#endif //__PROTOCOL_H