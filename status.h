#ifndef __STATUS_H

#define __STATUS_H


typedef enum
{
	eNoDataStatus 			= 0x00,
	eDataReady 				= 0x01,
	eTimedOut 				= 0x02,
	ePulseTooLong 			= 0x04,
	eCyclicTimer 			= 0x08,
	eReceivingCloneData 	= 0x10,
	eShooting 				= 0x20,
}EStatus;

extern bank1 byte status;

#define GetStatus() status
#define SetStatusBit(m) (status |= m)
#define ClearStatusBit(m) (status &=~m)
#define ResetStatusByte() (status = eNoDataStatus)





#endif //__STATUS_H