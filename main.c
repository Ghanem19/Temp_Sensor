/*
 * main.c
 *
 *  Created on: Jan 21, 2020
 *      Author: Ghanem
 */
/************************************************************************/
/*		                        Includes                 		        */
/************************************************************************/
#include "common_retval.h"
#include "interrupt.h"
#include "lcd.h"
#include <stdlib.h>
#include "temp_sensor.h"
#include "sos.h"

/************************************************************************/
/*		             			Tasks Prototypes	 	        	    */
/************************************************************************/
void lcdInitTask(void);
void sendStringTask(void);
void Trigg(void);
void displayTemp(void);
void readTempValue(void);
void displayTemp(void);
/************************************************************************/
/*		                     Global Variables	                 	    */
/************************************************************************/
uint16 temp = 0;
uint8 string[5];
str_taskInfo_t task4 = {displayTemp  ,20,1,0,TASK_BLOCKED};


int main (void)
{

	TempSensor_init();
	Sos_Init();

	str_taskInfo_t task1 = {lcdInitTask,1,10,0,TASK_WAITING};
	str_taskInfo_t task2 = {Trigg,1000,4,20,TASK_WAITING};
	str_taskInfo_t task3 = {readTempValue,1002,5,20,TASK_WAITING};
	str_taskInfo_t task5 = {sendStringTask,20,3,20,TASK_WAITING};
	Sos_CreateTask(&task1);
	Sos_CreateTask(&task2);
	Sos_CreateTask(&task3);
	Sos_CreateTask(&task4);
	Sos_CreateTask(&task5);
	sei();

	Sos_Run();


}
/************************************************************************/
/*		             		Tasks Implementation 	 	        	    */
/************************************************************************/

void lcdInitTask(void)
{
	if((ERR_NOT_FINISHED + LCD_BASE ) == LCD_init())
	{
	}
	else
	{
		Sos_DeleteTask(lcdInitTask);
	}
}
void sendStringTask(void)
{
	uint8 string[] = "Temp = ";
	if((ERR_NOT_FINISHED + LCD_BASE ) == LCD_sendString_RowCol(0,3,string))
	{

	}
	else
	{
		Sos_DeleteTask(sendStringTask);
	}

}


void Trigg(void)
{
	TempSensor_convTrigger();
}



void readTempValue(void)
{

	TempSensor_readValue(&temp);

		itoa(temp, string, 10);
		task4.enum_taskStake = TASK_READY;
}

void displayTemp(void)
{

		if((ERR_NOT_FINISHED + LCD_BASE )!= LCD_sendString_RowCol(0,10,string))
		{

		}
		else
		{
			task4.enum_taskStake = TASK_BLOCKED;
		}
}
