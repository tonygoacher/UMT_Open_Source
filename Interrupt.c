

#include <htc.h>
#include "types.h"
#include "interrupt.h"
#include "protocol.h"
#include "status.h"

typedef enum
{
	eIdle,
	eTimingStartPulse,
	eReceivingBit,
	eWaitingForSpacer,
	
}ERxState;

typedef enum
{
	eTxIdle,
	eTxStartPulse1,
	eTxStartPulse2,
	eTxStartPulse3,
	eTxStartPulse4,	// Thes 4 values must be numerically consequtive
	eTxInterBitDelay,
	eTxData,
	eTxData1,
	
}ETxState;


typedef enum
{
	eReserved = 0,
	eCloneData,
	eScoreDataPt1,
	eScoreDataPt2,
	eScoreDataPt3,
	
}ESystemMessages;

typedef enum
{
	
}EMessageData;

#define MESSAGE_HEADER	0x80
#define	NO_MORE_BITS	0
#define SHOT_PACKET_BIT_LENGTH	6	// 6 bits in the data of a shot packet
#define ONE_BYTE_BIT_LENGTH		8
#define DATA_BYTE_AND_TERMINATOR_BIT_LENGTH	16
#define CLONE_PACKET_BIT_LENGTH_HALF ((36 * ONE_BYTE_BIT_LENGTH)/2)


#define SYSTEM_MESSAGE_COMMAND	0x87
#define MESSAGE_TERMINATOR 0xE8



EProtocolState  eProtocolState;




byte rgbyDataBuffer[MAX_DATA_PACKET];
ERxState eState= eIdle;

 static byte byBitCounter;
near static byte byTimerValue;
near static byte byData;
static bit bHold ;

static unsigned int iCyclicTimer = 100; 
static unsigned int iCyclicTimerReset = 100;
#define ms250_SECOND_TIMER_RESET	416

typedef struct
{
	byte byHitLedTimer;
	byte byDelayTimer;
	unsigned int iSecondTimer;
}SSystemTimers;

SSystemTimers sSystemTimers;


static byte* pbyDataBuffer;


#pragma switch speed


#define SET_INTERRUPT_EDGE_RISING INTEDG=1
#define SET_INTERRUPT_EDGE_FALLING INTEDG=0
#define ENABLE_TIMER0_OVERFLOW_INT T0IE = 1
#define DISABLE_TIMER0_OVERFLOW_INT T0IE = 0
#define RESET_TIMER0	TMR0=0; 

// Pulse timings
// With current prescaler, the following TMR0 are obtained:
// 2400uS = 0x95 = 149(d)
// 1200uS = 0x4a = 74(d)
// 600uS = 0x25 = 37(d)
// To allow for oscillator drift, the max and min values below are set with a +- 15% tolerance
#define MIN_START	111
#define MAX_START	179
#define MIN_SPACER	29
#define MAX_SPACER  44
#define MIN_BIT		29		// A bit must be > 600uS and < 1200uS
#define	MAX_BIT		90
#define	MIN_1		59		// Minimum time for a '1'. Pulses less than this are not ones
#define	MAX_1		89		// Max time for a '1'. Pulses greater than this are not ones

// Protocol values
#define COMMAND_PACKET	0x80
#define COMMAND_PACKET_B	0x07
#define MESSAGE_PACKET_BITS	0x10
#define ONE_BYTE_BITS	8


// Tx controls
static byte byTxData;
static byte byTxBitCount;
static bank2 byte byShotByte1;
static bank2 byte byShotByte2;
static ETxState	eTxState = eTxIdle;




// Define bit counts
#define MAX_BITS	255

#pragma switch speed


#define GetTimerUs() (TMR0)

// PWM Control
#define USE_PWM	1
#ifdef USE_PWM
#define PWM_OFF  0 
#define PWM_ON  0b00011100
#define PWMON	{CCP1CON = PWM_ON;}
#define PWMOFF	{CCP1CON=PWM_OFF; RC5=0;}
#else
#define PWMON  RC5=1
#define PWMOFF RC5=0
#endif



// Inline macro to save on call stack during interrupt
void ResetRxStateMachine()\
{\
DISABLE_TIMER0_OVERFLOW_INT;\
eState = eIdle;ResetProtocol();\
pbyDataBuffer = rgbyDataBuffer;\
ClearStatusBit(eReceivingCloneData);\
SET_INTERRUPT_EDGE_FALLING;}









// Initialise the receive system
void RxInterruptInit()
{
	// Set INT pin as triggering on rising edge
	eState = eIdle;	// Reset stats machine
	byBitCounter = 0;
	INTF=0;
	T0IF=0;
	PWMOFF;
	DISABLE_TIMER0_OVERFLOW_INT;		// Turn off timer0 overflow interrupt
	ResetRxStateMachine();
	ClearStatusBit( eTimedOut);
	ClearStatusBit(ePulseTooLong);
	// Enable edge interrupt
	INTE=1;
}





// This interrupt fires on the change of the IR receive pin




#define BITS_PER_SHOT_PACKET 14
#define NEXT_SHOT_BYTE (BITS_PER_SHOT_PACKET - 8)



void interrupt GlobalISR(void)
{
		
	if(TMR1IF)			// Timer 1 generates 600uS interrupt
	{
		TMR1IF = 0;
  		TMR1H	 = 0xFB;
  		TMR1L	 = 0x70;

			iCyclicTimer--;
			if(!iCyclicTimer)
			{
				iCyclicTimer = iCyclicTimerReset;
				SetStatusBit(eCyclicTimer);
			}
		
			if(!(--sSystemTimers.iSecondTimer))
			{
			
				sSystemTimers.iSecondTimer = ms250_SECOND_TIMER_RESET;
				if(sSystemTimers.byHitLedTimer)
				{
					sSystemTimers.byHitLedTimer--;
				}
				if(sSystemTimers.byDelayTimer)
				{
					sSystemTimers.byDelayTimer--;
				}
			}
	switch(eTxState & 0x7f)
			{
// Timer interrupt occurs every 600uS, implement 4 states for start
// pulse of 2400uS eStartPulse1-eStartPulse4
				case	eTxStartPulse1:
						{
						// Enable PWM
						PWMON;
				
						// DEBUG
						//RC5=1;
						//_DEBUG
			
						eTxState = eTxStartPulse2;
					
						byTxBitCount = BITS_PER_SHOT_PACKET ;
						byTxData = byShotByte1;
						}
						break;	

				case	eTxStartPulse2:
				case	eTxStartPulse3:
				case	eTxStartPulse4:
						eTxState++;
						break;			// No action needed, just keep it high
		
			
					
				case	eTxInterBitDelay:
						{
						// Disable PWM
						// DEBUG
						//RC5=0;
						//_DEBUG
						PWMOFF;
		
						if(!byTxBitCount)
						{
							eTxState = eTxIdle;
						}
						else
						{
							eTxState = eTxData;	
						}
						}
						break;
				case	eTxData:
						{
							//Enable PWM
							// DEBUG
							//	RC5=1;
							//_DEBUG
							PWMON;
							if(byTxData & 0x80)
							{
								eTxState = eTxData1;
							}
							else
							{
								eTxState = eTxInterBitDelay;
							}
							byTxData <<=1;
							--byTxBitCount;
							if(byTxBitCount == NEXT_SHOT_BYTE)
							{
								byTxData = byShotByte2;	
							}
	
						}	
						break;
				case	eTxData1:
						{
							eTxState = eTxInterBitDelay;
						}
						break;
				case	eTxIdle:
						PWMOFF;
						break;
			}
	}


	// Check for edge interrupt enabled (intedg) and flagged (INTF)
	if((INTE && INTF)|| (T0IE && T0IF))
	{
	
		{
			if(T0IE && T0IF)
			{
				ResetRxStateMachine();
				SetStatusBit(eTimedOut);
				T0IF=0;
				return;
			}
		
			INTF=0;
			switch(eState)
			{
				// Hit this state when the starting pulse is first detected. Start a timer to time how long it is
				case	eIdle:	
						{
							RESET_TIMER0; 
							ENABLE_TIMER0_OVERFLOW_INT;
							SET_INTERRUPT_EDGE_RISING;		// The next interrupt is when the start pulse ends
							eState = eTimingStartPulse;
		
						}
						break;
		
				// Hit this state when the start pulse ends, if in range, move on to timing the spacer
				case	eTimingStartPulse:
						{
							byTimerValue = GetTimerUs();
						
							RESET_TIMER0;
							if(byTimerValue < MIN_START || byTimerValue > MAX_START)
							{
								// Ignore short pulses
								ResetRxStateMachine();
								break;
							}
		
							SET_INTERRUPT_EDGE_FALLING;	// Next interrupt is when the spacer gap ends
							eState = eWaitingForSpacer;	
							byData=0;
							byBitCounter=8;
		
		
						}
						break;
		
		
				case	eWaitingForSpacer:	// The spacer gap has ended, this rising edge is the start of a bit
						{
							// Spacer must be within spec to be passed
							byTimerValue = GetTimerUs();
							RESET_TIMER0;
							if(byTimerValue < MIN_SPACER || byTimerValue > MAX_SPACER)
							{
								// Ignore short pulses
								ResetRxStateMachine();
								
								break;
							}
		
						
		
							eState = eReceivingBit;
							SET_INTERRUPT_EDGE_RISING;	// When bit is 0, we know the length of it and thus 0 or 1
					
						}
						break;
		
				case	eReceivingBit:		// The edge has fallen so we can time this bit. 
						{
							byTimerValue = GetTimerUs();
					
							RESET_TIMER0;
		
						
							if(byTimerValue < MIN_BIT || byTimerValue > MAX_BIT)
							{
								// Ignore short pulses
								ResetRxStateMachine();
								
								break;
							}
							
							byData <<=1;	// Data  shifted in MSB first
						
							// If timings within '1' range then set the bit
							if(byTimerValue > MIN_1 && byTimerValue < MAX_1)
							{
								byData |= 1;
								
							}
							byBitCounter--;	// Decrement the number of bits received
							eState = eWaitingForSpacer; // More bits to come
							//	If a full byte has been received, store and inform the foreground
							if(!(byBitCounter & 7))
							{
				
								
								*pbyDataBuffer++ = byData;	// Store the data byte
								
								// If no more bits to receive, Need to check the next state
								if(!byBitCounter)
								{
		
									switch( eProtocolState)	
									{
										case	eHeader:
												{
													if(!(byData & MESSAGE_HEADER))	// If a shot packet, can quit early
													{
														eProtocolState = eComplete;
														byBitCounter = SHOT_PACKET_BIT_LENGTH;
														break;
													}
						
													// Must be a message packet, need at least another 8 bits
													// But system message requires special handling
													if(byData == SYSTEM_MESSAGE_COMMAND)
													{
													    eProtocolState = eSystemMessage;
														byBitCounter =  ONE_BYTE_BIT_LENGTH;	// Need to check the data byte before bit count is known
														break;
													}
													else
													{
														 eProtocolState = eMessageTerminator;		// 	Only data and a terminator byte to follow		
														
													}
													byBitCounter =  DATA_BYTE_AND_TERMINATOR_BIT_LENGTH;	// Enough bits to rx data byte and terminato							
												}	
												break;				
										case	eMessageTerminator:
														{
															// Last byte should have the value of the message terminator
															if(byData != MESSAGE_TERMINATOR)
															{
																// Signal bad data to foreground 
																SetStatusBit(ePulseTooLong);
																
															}
															else
															{
		
																SetStatusBit(eDataReady);
															}
															byBitCounter =  NO_MORE_BITS;			// No more bits to receive forces rx state machine to reset.
															eProtocolState = eHeader;
														}
														break;

										case 	eSystemMessage:					// The data byte determines the remaining bytes to get
														{
															// This software only processes the clone command
															if(byData == eCloneData)
															{
														  	    eProtocolState = eCloneMessageTerminator;
																byBitCounter =  ONE_BYTE_BIT_LENGTH;
															}
															else
															{
																SetStatusBit(ePulseTooLong);
																byBitCounter = NO_MORE_BITS;
															}
															// Resert protocol state machine
															eProtocolState = eHeader;				
														}
														break;
									  	case	eCloneMessageTerminator:		// Here we expect a terminator, but anticipate a further 36 bytes of data
														{
															if(byData == MESSAGE_TERMINATOR)	
															{
																															eProtocolState = eStateCloneData2;
																SetStatusBit(eReceivingCloneData);
																byBitCounter = CLONE_PACKET_BIT_LENGTH_HALF; // 288 bits needed so do it in two halves
															}
															else
															{
																SetStatusBit(ePulseTooLong);
																byBitCounter = NO_MORE_BITS; 
															}
														}
														break;
										case	eStateCloneData2:			// Receive the remaining half of the clone data
														{
													     	eProtocolState = eComplete;
															byBitCounter = CLONE_PACKET_BIT_LENGTH_HALF;
														}
														break;
										case 	eComplete:
														{
															SetStatusBit(eDataReady);
															eProtocolState = eHeader;
															byBitCounter = NO_MORE_BITS;
														}
														break;
										default:
														{
								
															SetStatusBit(ePulseTooLong);
															 eProtocolState = eHeader;
														}		
														byBitCounter = NO_MORE_BITS;		
									}
									if(!byBitCounter)	// Bits exhausted, packet is complete
									{
										ResetRxStateMachine();
									}
								}
								byData=0;
							}
						
							SET_INTERRUPT_EDGE_FALLING;
						}
						break;		
		
			}
		}
	} 
}









void SetShotByte1(byte byByte1)
{
	byShotByte1 = byByte1;
}

void SetShotByte2(byte byByte2)
{
// Need to shift this right twice as only 6 bits are sent for
// the second data 'byte' and it is mesb ms bit first

	byShotByte2 = byByte2 << 2;
}

byte GetHitLedTimer()
{
	return sSystemTimers.byHitLedTimer;
}

void SetHitLedTimer(byte byTimeIn250mS)
{

	T0IE=0;
	sSystemTimers.iSecondTimer = ms250_SECOND_TIMER_RESET;
	sSystemTimers.byHitLedTimer = byTimeIn250mS;
	T0IE=bHold;
}

byte GetDelayTimer()
{
	return sSystemTimers.byDelayTimer;
}

void SetDelayTimer(byte byTimeIn250mS)
{
	bHold = T0IE;
	T0IE-0;
	sSystemTimers.iSecondTimer = ms250_SECOND_TIMER_RESET;
	sSystemTimers.byDelayTimer = byTimeIn250mS;
	T0IE=bHold;
}

void SetCyclicTimer(int timerValue)
{
 	bHold = T0IE;
	T0IE = 0;
	iCyclicTimerReset = timerValue;
	T0IE=bHold;
}

void ResetCyclicTimer()
{
	bHold = T0IE;
	T0IE = 0;
	iCyclicTimer = iCyclicTimerReset;
	T0IE = bHold;
}

void Shoot()
{
	eTxState = eTxStartPulse1;
}

