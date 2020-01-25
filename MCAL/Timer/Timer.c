/*
 * timer.c
 *
 *  Created on: Dec 21, 2019
 *      Author: eng-m
 */



/************************************************************************/
/*		                        INCLUDES                 		        */
/************************************************************************/
#include "registers.h"
#include "Timer.h"
#include "common_macros.h"
#include "interrupt.h"
/************************************************************************/
/*		                        DEFINITIONS                 		        */
/************************************************************************/

/*TIFR BITS NAME*/
#define TOV0  (0x01)
#define TOV1  (0x04)
#define TOV2  (0x40)

/*TIMSK BITS Define*/
#define TOIE0 (0x01)
#define TOIE1 (0x04)
#define TOIE2 (0x40)

#define OCIE0 (0x02)
#define OCIE1 (0x10)
#define OCIE2 (0x80)

#define OCIE(CH) (CH) ? (OCIE1):(OCIE0)

#define TIMER0_COMP (0x08)
#define TIMER1_COMP (0x0800)
#define TIMER2_COMP (0x08)

#define COUNTER_OFFSET  (0x08)
#define STOP_TIMER (0xf8)
#define MAX_MODES 3

#define PRESCAL_MASK  0
#define PRESCAL_VALUE 1

#define ONE 1
#define TWO 2
/************************************************************************/
/*		                 TIMER STATIC VARIABLES          		        */
/************************************************************************/
uint8 gau8_TimerInitialized[3] = {0};
void (*TimersComper_CBF[MAX_CH])() = {NULL,NULL,NULL};
static uint8 gau8_TimerPreScaler[MAX_CH][TWO] = {{0}};
static uint8 gau8_CounterType[2] = {0};

/************************************************************************/
/*		                		TIMER ISR                  		        */
/************************************************************************/

ISR(TIMER0_COMP_vect)
{
	/* check if the address of function is assigned to the pointer or not.*/
	if(	TimersComper_CBF[TIMER_CH0] != NULL)
	{
		TimersComper_CBF[TIMER_CH0]();

	}
}

ISR(TIMER1_COMPA_vect)
{
	/* check if the address of function is assigned to the pointer or not.*/
	if(	TimersComper_CBF[TIMER_CH1] != NULL)
	{
		TimersComper_CBF[TIMER_CH1]();
	}

}
ISR(TIMER2_COMP_vect)
{
	/* check if the address of function is assigned to the pointer or not.*/
	if(	TimersComper_CBF[TIMER_CH2] != NULL)
	{
		TimersComper_CBF[TIMER_CH2]();
	}

}

/************************************************************************/
/*		                TIMER FUNCTIONS' DEFINITION     		        */
/************************************************************************/


/**
 * Input: Pointer to a structure contains the information needed to initialize the timer.
 * Output:
 * In/Out:
 * Return: The error status of the function.
 * Description: Initiates the module.
 *
 */
ERR_STATUS Timer_Init(Timer_cfg_s* Timer_cfg){
	/* check in the parameters of Timer_cfg are in their range or not.*/

	if((Timer_cfg != NULL) && (gau8_TimerInitialized[Timer_cfg->Timer_CH_NO] == 0)  \
	    && Timer_cfg->Timer_Mode < MAX_MODES && (Timer_cfg->Timer_Polling_Or_Interrupt <= ONE)\
		&& (Timer_cfg->Timer_Prescaler <= TIMER_PRESCALER_1024))
	{
		/*	look up table for timer 0, and timer 1*/
		uint16 au16_PwmPrescalerTable[7][2] = {{0x01,1},{0x02,8},{0,0},{0x03,64},{0,0},{0x04,265},{0x05,1024}};
		/* look up table for timer 2*/
		uint16 au16_PwmPrescalerTimer2[7] = {1,8,32,64,128,265,1024};



		/*Timer0 and Timer 1 have the same value of prescal and counter so work initiate them are similar */
		if(Timer_cfg->Timer_CH_NO == TIMER_CH0 || Timer_cfg->Timer_CH_NO == TIMER_CH1)
		{
				if(Timer_cfg->Timer_CH_NO == TIMER_CH0)
				{
					/* Overflow mode
						 * TCCR0:WGM00 = 0
						 * 		 WGM01 = 1
						 * 		 COM0 1:0 = 0
						 * 		 FOC0 = 0
						 * 		 set clk 0 at first. then apply it at start function.
						 * */
					TCCR0 = TIMER0_COMP;
				}
				else
				{
					/*TCCR1 register
					 * mode 0 wave generation
					 * 		WGM1 {3,1,0}  = 0
					 *		WGM12 = 1
					 * 		TOP = 0xffff
					 * Compare Mode CTC:
					 * COM1A/B = 0 , COM0A/B = 0*/
					TCCR1 = TIMER1_COMP;
				}
				/* Set the mask if interrupt required */
				if(Timer_cfg->Timer_Polling_Or_Interrupt == TIMER_INTERRUPT_MODE)
				{
					SET_BIT(TIMSK,OCIE(Timer_cfg->Timer_CH_NO));
					/* pass the function address to the associated pointer to function */
					TimersComper_CBF[Timer_cfg->Timer_CH_NO] = Timer_cfg->Timer_Cbk_ptr;
				}
				else
				{
					CLR_BIT(TIMSK,OCIE(Timer_cfg->Timer_CH_NO));
				}

				/* if one of counter modes is selected */
				if(Timer_cfg->Timer_Mode != TIMER_MODE)
				{
					if(Timer_cfg->Timer_Prescaler != TIMER_PRESCALER_NO)
					{
						/* there is no prescaler applicable on counter modes.*/
						return E_NOK;
					}
					else
					{
						/* at falling edge need -> 110 = 6  8 - #define of falling
						 * at Rising edge need 	-> 111 = 7  8 - #define of rising
						 * */
						gau8_CounterType[Timer_cfg->Timer_CH_NO] = COUNTER_OFFSET - Timer_cfg->Timer_Mode;
					}
				}
				else
				{
					/*for Timer mode at timer 0:1 there is no 32 or 128 precaler. So map the coming value and set it at
					 * global variable to use prescaler at start function.
					 * */
					gau8_TimerPreScaler[Timer_cfg->Timer_CH_NO][PRESCAL_MASK] =  au16_PwmPrescalerTable[Timer_cfg->Timer_Prescaler-ONE][PRESCAL_MASK];
					gau8_TimerPreScaler[Timer_cfg->Timer_CH_NO][PRESCAL_VALUE] =  au16_PwmPrescalerTable[Timer_cfg->Timer_Prescaler-ONE][PRESCAL_VALUE];


				}
		}

		/*	Timer 2 Initialization */
		else if(Timer_cfg->Timer_CH_NO == TIMER_CH2)
		{
			/* Overflow mode
					*  TCCR0:WGM20 = 0
					* 		 WGM21 = 1
					* 		 COM2 1:0 = 0
					* 		 FOC2 = 0
					* 		 set clk 0 at first. then apply it at start function.
				    */
				TCCR2 = TIMER2_COMP;
				/*set mask of interrupt if required */
				if(Timer_cfg->Timer_Polling_Or_Interrupt == TIMER_INTERRUPT_MODE){
					SET_BIT(TIMSK, OCIE2);
					/* pass the function address to Timer 2 pointer to function */
					TimersComper_CBF[Timer_cfg->Timer_CH_NO] = Timer_cfg->Timer_Cbk_ptr;
				}
				else{
					CLR_BIT(TIMSK, OCIE2);
				}
				if(Timer_cfg->Timer_Mode != TIMER_MODE){
					/* counter mode not implemented at TIMER 2*/
					return E_NOK;
				}
				else{
					/* assign the value of prescaler to global variable to use it at start function. */
					gau8_TimerPreScaler[TIMER_CH2][PRESCAL_MASK]  = Timer_cfg->Timer_Prescaler;
					gau8_TimerPreScaler[TIMER_CH2][PRESCAL_VALUE] = au16_PwmPrescalerTimer2[Timer_cfg->Timer_Prescaler-ONE];
				}
		}
		else
		{
			/* if timer channel not exist */
			return E_NOK;
		}

	}
	else{
		/* if any of initial check are false return error*/
		return E_NOK;

	}
	/*
	 * set flag to timer to indicate that it's initialized successfully
	 */
	gau8_TimerInitialized[Timer_cfg->Timer_CH_NO] = TRUE;
	return E_OK;
}


/**
 * Input:
 * 	Timer_CH_NO: The channel number of the timer needed to be started.
 *	Timer_Count: The start value of the timer.
 * Output:
 * In/Out:
 * Return: The error status of the function.
 * Description: This function starts the selected timer.
 */
ERR_STATUS Timer_Start(uint8 Timer_CH_NO, uint16 Timer_Count){
	/*	check if timer is already initialized or not*/
	if(Timer_CH_NO < MAX_CH && gau8_TimerInitialized[Timer_CH_NO] == 1){
		if( ((Timer_CH_NO == TIMER_CH2) || (Timer_CH_NO == TIMER_CH0)) &&  Timer_Count>TIMER_8BITS_OVF){
			/*check if counts are in range */
			return E_NOK;
		}
		else{
			switch(Timer_CH_NO){
			/*for the selected timer load the TCNT wit initial value
			 * set clk to TCCR0 by assigned prescaler if TIMER mode is selected.
			 * if counter selected choose if counter up or down*/
				case TIMER_CH0:
					OCR0 = Timer_Count;
					if(gau8_CounterType[Timer_CH_NO] == 0){
						TCCR0 |=  gau8_TimerPreScaler[TIMER_CH0][PRESCAL_MASK];
					}
					else
					{
						TCCR0 |=  gau8_CounterType[TIMER_CH0];
					}
					break;

				case TIMER_CH1:
					OCR1A = Timer_Count;
					if(gau8_CounterType[TIMER_CH1] == 0){
						TCCR1 |=  gau8_TimerPreScaler[TIMER_CH1][PRESCAL_MASK];
					}
					else
					{
						TCCR1 |=  gau8_CounterType[TIMER_CH1];
					}
					break;

				case TIMER_CH2:
					OCR2 = Timer_Count;
					TCCR2 |=  gau8_TimerPreScaler[TIMER_CH2][PRESCAL_MASK];
					break;

				default:
					/* if channel not exist selected*/
					return E_NOK;
			}

		}

	}
	else{
		/* initialization check failed */
		return E_NOK;
	}
/*  Timer start successfully  */
	return E_OK;
}


/*
 * Input:
 * 	Timer_CH_NO: The channel number of the timer needed to get its status.
 * Output:
 * 	Data: A variable of type bool returns if the flag of the timer is raised or not.
 * In/Out:
 * Return: The error status of the function.
 * Description: This function is used to return if the flag of the timer is raised or not.
 */
ERR_STATUS Timer_GetStatus(uint8 Timer_CH_NO, bool* Data){
	/*
	 * return the value of TOV flag if set or not
	 * */
	uint8 u8_overflowFlag = 0;
	switch(Timer_CH_NO)
	{
		case TIMER_CH0:
			*Data = GET_BIT(TIFR,TOV0);
			u8_overflowFlag = TOV0;
			break;

		case TIMER_CH1:
			*Data = GET_BIT(TIFR,TOV1);
			u8_overflowFlag = TOV1;
			break;

		case TIMER_CH2:
		    *Data = GET_BIT(TIFR,TOV2);
		    u8_overflowFlag = TOV2;
		    break;
		default:
			return E_NOK;
	}
	if(*Data == TRUE)
	{
		/* clear the flag before return */
		CLR_BIT(TIFR,u8_overflowFlag);
	}
	else
	{

	}

	return E_OK;
}


/**
 * Input:
 * 	Timer_CH_NO: The channel number of the timer needed to get its value.
 * Output:
 * 	Data: This is the output variable of the function which holds the value of the timer.
 * In/Out:
 * Return: The error status of the function.
 * Description: This function is used to return the value of the TCNT REG.
 *
 */
ERR_STATUS Timer_GetValue(uint8 Timer_CH_NO, uint16* Data){
	switch(Timer_CH_NO)
		{
		/* for the selected timer return the value of TCNT register*/
			case TIMER_CH0:
				*Data = TCNT0;
				break;

			case TIMER_CH1:
				*Data = TCNT1;
				break;

			case TIMER_CH2:
			    *Data = TCNT2;
			    break;
			default:
				/* channel number is not in range*/
				return E_NOK;
		}

	return E_OK;

}

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
ERR_STATUS Timer_GetTickTime(uint8 Timer_CH_NO, uint16* TickTime){
	if(Timer_CH_NO < MAX_CH && gau8_TimerInitialized[Timer_CH_NO] == TRUE)
	{
		*TickTime = ((gau8_TimerPreScaler[Timer_CH_NO][PRESCAL_VALUE])/16);

	}
	else
	{
		return E_NOK;
	}

	return E_OK;

}


/**
 * Input:
 * 	Timer_CH_NO: The channel number of the timer needed to be stopped.
 * Output:
 * In/Out:
 * Return: The error status of the function.
 * Description: This function stops the needed timer.
 *
 */
ERR_STATUS Timer_Stop(uint8 Timer_CH_NO)
{
	switch(Timer_CH_NO)
	{
		case TIMER_CH0:
			TCCR0 &= STOP_TIMER ;
			break;

		case TIMER_CH1:
			TCCR1 &= STOP_TIMER;
			break;

		case TIMER_CH2:
			TCCR2 &=  STOP_TIMER;
			break;

		default:
			return E_NOK;
	}

	return E_OK;
}
/**
 * Input:
 * 	Timer_CH_NO: The channel number of the timer needed to be stopped.
 * Output:
 * In/Out:
 * Return: The error status of the function.
 * Description: This function deinit the needed timer.
 *
 */
ERR_STATUS Timer_Deinit(uint8 Timer_CH_NO)
{
	switch(Timer_CH_NO)
	{
		case TIMER_CH0:
			TCNT0 = 0;
			TCCR0 &= STOP_TIMER ;
			CLR_BIT(TIMSK, OCIE0);

			break;

		case TIMER_CH1:
			TCNT1 = 0;
			TCCR1 &= STOP_TIMER;
			CLR_BIT(TIMSK, OCIE1);
			break;

		case TIMER_CH2:
			TCNT2 = 0;
			TCCR2 &=  STOP_TIMER;
			CLR_BIT(TIMSK, OCIE2);
			break;

		default:
			return E_NOK;
	}

	TimersComper_CBF[Timer_CH_NO] = NULL;
	gau8_CounterType[Timer_CH_NO] = 0;
	gau8_TimerInitialized[Timer_CH_NO] = 0;
	gau8_TimerPreScaler[Timer_CH_NO][PRESCAL_MASK] = 0;
	gau8_TimerPreScaler[Timer_CH_NO][PRESCAL_VALUE] = 0;


	return E_OK;
}

