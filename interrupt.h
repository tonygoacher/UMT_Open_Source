#ifndef __INTERRUPT_H
#define __INTERRUPT_H

enum
{
	ePacketShotPlayer,
	ePacketShotData,
};



void interrupt GlobalISR(void);
void RxInterruptInit();

#define MAX_DATA_PACKET 39
extern byte rgbyDataBuffer[MAX_DATA_PACKET];

void ResetRxStateMachine();
void RxInterruptInit();

void SetShotByte1(byte byByte1);
void SetShotByte2(byte byByte2);

byte GetHitLedTimer();
void SetHitLedTimer(byte byTimeIn250mS);
byte GetDelayTimer();
void SetDelayTimer(byte byTimeIn250mS);
void SetCyclicTimer(int timerValue);
void ResetCyclicTimer();


void Shoot();

#endif //__INTERRUPT_H


