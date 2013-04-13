//
// Module to process the IR protocol
//
#include "types.h"
#include "interrupt.h"
#include "protocol.h"

#pragma switch space

static byte byPower;
static SMessage* pData;

void ProcessIrPacket()
{
	pData = (SMessage*)GetBuffer();
	// If a shot packet
	if(!(pData->shotPacket.playerId & SYSTEM_MESSAGE))
	{
		byPower = pData->shotPacket.shotPower;
		pData->shotPacket.shotPower = byPower;
	}
	
}