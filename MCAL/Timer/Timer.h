/*
 * Timer.h
 *
 *  Created on: Dec 21, 2019
 *      Author: Zayed & Saad
 */

#ifndef TIMER_H_
#define TIMER_H_

/************************************************************************/
/*				 INCLUDES			        */
/************************************************************************/

#include "std_types.h"

/************************************************************************/
/*				 DEFINES			        */
/************************************************************************/


#define		TIMER_CH0					0
#define		TIMER_CH1					1
#define		TIMER_CH2					2

#define		TIMER_MODE					    0
#define		COUNTER_RISING_EDGE				1
#define		COUNTER_FALLING_EDGE			2

#define		TIMER_POLLING_MODE				0
#define		TIMER_INTERRUPT_MODE			1

#define		TIMER_NO_CLOCK					0
#define		TIMER_PRESCALER_NO				1
#define		TIMER_PRESCALER_8				2
#define		TIMER_PRESCALER_32				3
#define		TIMER_PRESCALER_64				4
#define		TIMER_PRESCALER_128				5
#define		TIMER_PRESCALER_256				6
#define		TIMER_PRESCALER_1024			7

#define TIMER_8BITS_OVF (0x00ff)
#define TIMER_16BITS_OVF (0xffff)

#define MAX_CH 3


/************************************************************************/
/*			  Structures Definitions		        */
/************************************************************************/

typedef struct Timer_cfg_s
{
	uint8 Timer_CH_NO;
	uint8 Timer_Mode;
	uint8 Timer_Polling_Or_Interrupt;
	uint8  Timer_Prescaler;
	void  (*Timer_Cbk_ptr)(void);

}Timer_cfg_s;


extern uint8 gau8_TimerInitialized[3];
/************************************************************************/
/*		         TIMER FUNCTIONS' PROTOTYPES		        */
/************************************************************************/


/**
 * Input: Pointer to a structure contains the information needed to initialize the timer.
 * Output:
 * In/Out:
 * Return: The error status of the function.
 * Description: Initiates the module.
 *
 */
ERR_STATUS Timer_Init(Timer_cfg_s* Timer_cfg);

/**
 * Input:
 * 	Timer_CH_NO: The channel number of the timer needed to be started.
 *	Timer_Count: The start value of the timer.
 * Output:
 * In/Out:
 * Return: The error status of the function.
 * Description: This function strats the needed timer.
 *
 */
ERR_STATUS Timer_Start(uint8 Timer_CH_NO, uint16 Timer_Count);

/**
 * Input:
 * 	Timer_CH_NO: The channel number of the timer needed to be stopped.
 * Output:
 * In/Out:
 * Return: The error status of the function.
 * Description: This function stops the needed timer.
 *
 */
ERR_STATUS Timer_Stop(uint8 Timer_CH_NO);
/**
 * Input:
 * 	Timer_CH_NO: The channel number of the timer needed to be stopped.
 * Output:
 * In/Out:
 * Return: The error status of the function.
 * Description: This function deinit the needed timer.
 *
 */
ERR_STATUS Timer_Deinit(uint8 Timer_CH_NO);

/**
 * Input:
 * 	Timer_CH_NO: The channel number of the timer needed to get its status.
 * Output:
 * 	Data: A variable of type bool returns if the flag of the timer is raised or not.
 * In/Out:
 * Return: The error status of the function.
 * Description: This function is used to return if the flag of the timer is raised or not.
 *
 */
ERR_STATUS Timer_GetStatus(uint8 Timer_CH_NO, bool* Data);

/**
 * Input:
 * 	Timer_CH_NO: The channel number of the timer needed to get its value.
 * Output:
 * 	Data: This is the output variable of the function which holds the value of the timer.
 * In/Out:
 * Return: The error status of the function.
 * Description: This function is used to return the value of the timer.
 *
 */
ERR_STATUS Timer_GetValue(uint8 Timer_CH_NO, uint16* Data);


/**
 * Input:
 * 	Timer_CH_NO: The channel number of the timer needed to get its value.
 * Output:
 * 	TickTime: This is the output variable of the function which holds the value of the timer tick.
 * In/Out:
 * Return: The error status of the function.
 * Description: This function is used to return the value of single timer tick by microseconds.
 *
 */
ERR_STATUS Timer_GetTickTime(uint8 Timer_CH_NO, uint16* TickTime);








#endif /* TIMER_H_ */
