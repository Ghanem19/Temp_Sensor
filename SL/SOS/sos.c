/*
 * TMU.c
 *
 *  Created on: Jan 11, 2020
 *      Author: Ghanem
 */


/************************************************************************/
/*		                        Includes                 		        */
/************************************************************************/
#include "sos.h"
#include "Timer.h"
#include "sos_cfg.h"
#include "sos_lcfg.h"
#include "sleep_mode.h"

/************************************************************************/
/*		                        Defines                 		        */
/************************************************************************/
#define EMPTY    0
#define MICRO_TO_MILI_CONV 1000

/************************************************************************/
/*		                        Global Variables                 	    */
/************************************************************************/

str_taskInfo_t gastr_TasksQueue[SOS_BUFFER_SIZE] = {{0}} , gstr_deletedTask = {NULL,0,0,0,TASK_WAITING};
static uint8  gsu8_tasksBufferCapacity = 0;
volatile uint8 gvu8_timerISRCounter = 0;
static enum_initState_t gsenum_sosState = UNINITIALIZED;
/************************************************************************/
/*		             	 Static Functions Prototypes               	    */
/************************************************************************/
/*
 * function:	Sos_sysTickISR_CBK
 * Description: passed to Timer_Init function to be executed at each ISR */
static void Sos_sysTickISR_CBK(void);
/*
 * Function: Sos_sortTasksBuffer.
 *
 * Description: Sort the Tasks buffer by bubble sort according to their priority to be called directly
 * by the scheduler */
static void Sos_sortTasksBuffer(void);
/************************************************************************/
/*		                 External Functions' Implementation       	    */
/************************************************************************/


/*
 * Function: Sos_Init
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: initialize the required timer to be used in SOS ticks counter with the selected prescal
 * 				compare value should be set to achieve the resolution.
 */
ERR_STATUS Sos_Init(void)
{
	/* suppose to check if timer could achieve the required resolution */
	ERR_STATUS sosInitError = ERR_SUCCESS;
	uint8 u8_index ;
	/* check if OS is already initialized*/
	if(gsenum_sosState == INITIALIZED)
	{
		sosInitError = ERR_ALREADY_INITIALIZED;
	}
	else
	{

		/* set the timer initial values */
		Timer_cfg_s str_timerCfg = {
				gstr_sosConfig.u8_timerId,
				TIMER_MODE,
				TIMER_INTERRUPT_MODE,
				TIMER_PRESCALER_64,
				Sos_sysTickISR_CBK };
		/* Initialize timer */
		if(E_NOK == Timer_Init(&str_timerCfg))
		{
			/* Error in timer initialization */
			sosInitError = ERR_TIMER;
		}
		else
		{
			/* initialize the sleep mode */
			CPU_sleepInit();
			/* initialize the whole global variables */
			/* clear the whole buffer */
			for(u8_index = 0;u8_index < SOS_BUFFER_SIZE; u8_index++)
			{
				gastr_TasksQueue[u8_index] = gstr_deletedTask;
			}
			gsu8_tasksBufferCapacity = 0;
			gvu8_timerISRCounter = 0;
			/* convert OS to initialized state */
			gsenum_sosState = INITIALIZED;
		}

	}
	return sosInitError + SOS_BASE;
}
/*
 * Function: Sos_CreateTask
 *
 * Inputs: 	*pstr_newTask -> pointer to structure of type str_taskInfo_t which contain
 * 				 pointer to task function address that would added to buffer.
 * 				 the period which task would execute in it.
 * 				 priority of task. rest of elements woudn't be used in create function.
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: add the passed task to buffer to be executed within certain periodic.
 */
ERR_STATUS Sos_CreateTask(str_taskInfo_t *pstr_newTask)
{
	ERR_STATUS sosCreatTaskErr = ERR_SUCCESS;
	/* check for null pointer as pointer to structure isn't assigned to initialized structure address*/
	if(pstr_newTask == NULL)
	{
		sosCreatTaskErr = ERR_NULL_PTR;
	}
	else
	{
		/* check if SOS is initialized to creatTask0 it*/
		if(gsenum_sosState == UNINITIALIZED)
		{
			sosCreatTaskErr = ERR_NOTINITIALIZED;
		}
		else
		{
			/* check if buffer not full to add task*/
			if(gsu8_tasksBufferCapacity < SOS_BUFFER_SIZE)
			{

				gastr_TasksQueue[gsu8_tasksBufferCapacity] = *pstr_newTask;
				/* increase the buffer capacity after add the new task. */
				gsu8_tasksBufferCapacity++;
			}
			else
			{
				//return error as there is not enough space
				sosCreatTaskErr =  ERR_RESOURCE_NOT_FOUND;
			}

		}

	}
	return sosCreatTaskErr + SOS_BASE;
}

/*
 * Function: Sos_Run
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: after each timer ISR executed the scheduler check which function is ready to be executed from buffer then call it.
 */
ERR_STATUS Sos_Run(void)
{
	/*local variables initialization */
	uint8 u8_index = 0;
	uint16 u16_timeOfTick = 0,u16_numOfTicks = 0;
	static uint16 gu16_sysTicks = 0;
	ERR_STATUS sosSchedulerErr = ERR_SUCCESS;


	/* Check if the SOS already initialized or not */
	if(gsenum_sosState == UNINITIALIZED)
	{
		sosSchedulerErr = ERR_NOTINITIALIZED;
	}
	else
	{
		/* calculate the suitable ticks to get the required resolution. */
		if((E_NOK == Timer_GetTickTime(gstr_sosConfig.u8_timerId,&u16_timeOfTick)) || (u16_timeOfTick == 0))
		{
			sosSchedulerErr = ERR_TIMER;
		}
		else

		{
			/* calculate ticks that would set the resolution */
			u16_numOfTicks = MICRO_TO_MILI_CONV / u16_timeOfTick;

			/* start timer and load Compare with ticks suitable to achieve resolution. */
			if(E_NOK == Timer_Start(gstr_sosConfig.u8_timerId,u16_numOfTicks))
			{
				sosSchedulerErr = ERR_TIMER;
			}
			else
			{
				/* sort the buffer tasks according to their priority before the super loop */
				Sos_sortTasksBuffer();
			}

		}

	}
	/* SUPER LOOP STARTING */
	while(1)
	{
		/* check only if the ISR timer counter have been increased */
		while(gvu8_timerISRCounter > 0)
		{
			/* increment sysTick as timer ISR flag rose */
			gu16_sysTicks++;
			/* loop to get ready Tasks */
			for(u8_index = 0; u8_index <= gsu8_tasksBufferCapacity; u8_index++)
			{
				/* check if there is function to be executed at current sysTick*/
				if( ((gu16_sysTicks) - (gastr_TasksQueue[u8_index].u8_startTime)) % (gastr_TasksQueue[u8_index].u16_period) == 0)
				{
					gastr_TasksQueue[u8_index].enum_taskStake = TASK_READY;
				}
				else
				{
					/* Do nothing */
				}
			}

			for(u8_index = 0; u8_index <= gsu8_tasksBufferCapacity; u8_index++)
			{
				if(	gastr_TasksQueue[u8_index].enum_taskStake == TASK_READY)
				{
					/* run function which is ready */
					gastr_TasksQueue[u8_index].ptr_TaskFunc(); /* RUNNING .....*/
					/* return function to waiting state*/
					gastr_TasksQueue[u8_index].enum_taskStake = TASK_WAITING;
				}
			}
			/* decrement ISR counter to detect next ISR*/
			gvu8_timerISRCounter--;
		}

		/* sleep the CPU until next systick ISR*/
		CPU_sleep();
	}

	return sosSchedulerErr + SOS_BASE;
}


/*
 * Function: Sos_DeleteTask
 *
 * Inputs: *ptr_Function -> pointer to task function address that would removed from buffer.
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: remove the passed function task from buffer. if it was last function in buffer stop the timer.
 */
ERR_STATUS Sos_DeleteTask(ptr_Func_t ptr_stopFunc)
{
	uint8 u8_bufferIndex = 0;
	ERR_STATUS sosDeleteErr = ERR_SUCCESS;
	/* Check if the TMU already initialized or not */
	if(gsenum_sosState == UNINITIALIZED)
	{
		sosDeleteErr = ERR_NOTINITIALIZED;
	}
	else
	{
		/* use linear search to get the index of passed function at the buffer*/
		while((u8_bufferIndex <= gsu8_tasksBufferCapacity) && (ptr_stopFunc != gastr_TasksQueue[u8_bufferIndex].ptr_TaskFunc))
		{
				u8_bufferIndex++;
		}
		if(u8_bufferIndex <= gsu8_tasksBufferCapacity)
		{
			/* delete the task and sort the buffer again to swap the deleted task at end of buffer*/
			gastr_TasksQueue[u8_bufferIndex] = gstr_deletedTask;
			Sos_sortTasksBuffer();
			if(gsu8_tasksBufferCapacity > EMPTY)
			{
				gsu8_tasksBufferCapacity--;
			}
			else
			{
				/* if there is only one Task at the buffer or it's empty
					suppose to stop timer as there is no tasks to be executed */
			}
		}
		else
		{
			/* function isn't found in the buffer */
			sosDeleteErr = ERR_INVALID_ARGU;
		}
	}

	return sosDeleteErr + SOS_BASE;
}

/*
 * Function: Sos_sortTasksBuffer
 * Description: function uses bubble sort to sort tasks in the buffer according to their priority.
 */
static void Sos_sortTasksBuffer(void)
{
	/* local variables initialization */
	uint8 u8_innerbufferIndex = 0,u8_outterbufferIndex;
	bool u8_swapped = FALSE;
	str_taskInfo_t str_tempTask = {0};
/**/
	for (u8_outterbufferIndex = 0; u8_outterbufferIndex < gsu8_tasksBufferCapacity; u8_outterbufferIndex++)
	{
		u8_swapped = FALSE;
		for (u8_innerbufferIndex = 0; u8_innerbufferIndex < (gsu8_tasksBufferCapacity - u8_outterbufferIndex); u8_innerbufferIndex++)
		{
			if (gastr_TasksQueue[u8_innerbufferIndex].u8_priority < gastr_TasksQueue[u8_innerbufferIndex+1].u8_priority)
			{
				/* swap tasks current task with next one if the cuttent task has lower priority.  */
				str_tempTask = gastr_TasksQueue[u8_innerbufferIndex];
				gastr_TasksQueue[u8_innerbufferIndex] =  gastr_TasksQueue[u8_innerbufferIndex + 1];
				gastr_TasksQueue[u8_innerbufferIndex + 1] = str_tempTask;
				u8_swapped = TRUE;
			}
		}
		// IF no two elements were swapped by inner loop, then break
		if (u8_swapped == FALSE)
		{
			/* no need to swap again */
			break;
		}
	}

}

/*
 * function:	Sos_sysTickISR_CBK
 * Description: passed to Timer_Init function to be executed at each ISR */
void Sos_sysTickISR_CBK(void)
{
	 gvu8_timerISRCounter++;
}
