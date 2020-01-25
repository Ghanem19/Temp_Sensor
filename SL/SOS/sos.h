/*
 * tmu.h
 *
 *  Created on: Jan 11, 2020
 *      Author: Ghanem
 */

#ifndef SL_SOS_SOS_H_
#define SL_SOS_SOS_H_
/************************************************************************/
/*		                        Includes                 		        */
/************************************************************************/
#include "std_types.h"
#include "common_retval.h"
/************************************************************************/
/*		                        Defines                 		        */
/************************************************************************/
#define	TASK_WAITING	(enum_taskState_t)(0)
#define TASK_READY		(enum_taskState_t)(1)
/************************************************************************/
/*		                     Explicit Types                		        */
/************************************************************************/
typedef uint8 enum_taskState_t;
typedef struct{
	ptr_Func_t ptr_TaskFunc;
	uint16 u16_period;
	uint8 u8_priority;
	uint8 u8_startTime;
	enum_taskState_t enum_taskStake;

}str_taskInfo_t;
/************************************************************************/
/*		             External Functions  PROTOTYPES       		        */
/************************************************************************/

/*
 * Function: Sos_Init
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: initialize the required timer to be used in SOS ticks counter with the selected prescal
 * 				compare value should be set to achieve the resolution.
 */
extern ERR_STATUS Sos_Init(void);
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
extern ERR_STATUS Sos_CreateTask(str_taskInfo_t *pstr_newTask);

/*
 * Function: Sos_Run
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: after each timer ISR executed the scheduler check which function is ready to be executed from buffer then call it.
 */
extern ERR_STATUS Sos_Run(void);
/*
 * Function: Sos_DeleteTask
 *
 * Inputs: *ptr_Function -> pointer to task function address that would removed from buffer.
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: remove the passed function task from buffer. if it was last function in buffer stop the timer.
 */
extern ERR_STATUS Sos_DeleteTask(ptr_Func_t ptr_stopFunc);

#endif /* SL_SOS_SOS_H_ */
