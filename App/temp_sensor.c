/*
 * Edit temp_sensor file 
 * temp_sensor.c
 *  
 *  Created on: Jan 26, 2020
 *      Author: Ghanem
 */
/************************************************************************/
/*		                        Includes                 		        */
/************************************************************************/
#include "common_retval.h"
#include "tempSensor.h"
#include "adc.h"

/************************************************************************/
/*		             			Defines				 	        	    */
/************************************************************************/
#define CONVER_NOT_COMPLETE (0U)
#define CONVER_COMPLETE 	(1U)
/************************************************************************/
/*		                     Global Variables	                 	    */
/************************************************************************/
uint8 gu8_adcCompleteFlag = CONVER_NOT_COMPLETE;
uint16 u16_adcConvData = 0;
enum_initState_t genum_tempSensorState = UNINITIALIZED;
/************************************************************************/
/*		                Static Functions Prototypes                	    */
/************************************************************************/
/*
 * Description: function call Adc_getConvertedValue. function to read the
 * ADCH and ADCL values which contain the converted digital data.
 * */
static void TempSensor_adcCompleteConv_CBK(void);

/************************************************************************/
/*		             External Functions Implementation 	        	    */
/************************************************************************/

/*
 * Function: TempSensor_init.
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: initialize the ADC module to interface with LM34/35 sensor.
 */
ERR_STATUS	TempSensor_init(void)
{
	/* initialize the error status */
	ERR_STATUS initErr = ERR_SUCCESS;
	if(INITIALIZED == genum_tempSensorState )
	{
		genum_tempSensorState = ERR_ALREADY_INITIALIZED;
	}
	else
	{
		/* initialize the ADC module */
		str_adcCfg_t str_adcCfg = {
					ADC_VREF_INT,
					ADC_RIGHT_ADGJUST,
					ADC_PRESCALER_128,
					ADC_RESOLUTION_10,
					ADC_INTERRUPT,
					ADC_SINGLE_CONV,
					TempSensor_adcCompleteConv_CBK};

		initErr = Adc_Init(&str_adcCfg) - ADC_BASE;

		if(ERR_SUCCESS == initErr)
		{
			/* initialize the global variable */
			gu8_adcCompleteFlag = CONVER_NOT_COMPLETE;
			u16_adcConvData = 0;
			/* indicate than module is initialized */
			genum_tempSensorState = INITIALIZED;
		}
		else
		{
			/* Do nothing */
		}

	}
	return initErr + TEMP_SENSOR_BASE;
}
/*
 * Function: TempSensor_convTrigger.
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: trigger the ADC to start converting the analog data.
 */
ERR_STATUS	TempSensor_convTrigger(void)
{
	/* initialize the error status */
	ERR_STATUS triggerErr = ERR_SUCCESS;
	if(UNINITIALIZED == genum_tempSensorState)
	{
		/* not initialized */
		triggerErr = ERR_NOTINITIALIZED;
	}
	else
	{
		/* start conversion */
		triggerErr = Adc_StartConversion(ADC_CHANNEL_0) - ADC_BASE;
	}

	return triggerErr + TEMP_SENSOR_BASE;
}
/*
 * Function: TempSensor_readValue.
 *
 * OUTPUTS: *u8_tempValue -> data returned to user by that pointer
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: get the converted digital value when the conversion complete .
 */
ERR_STATUS	TempSensor_readValue(uint16 *u8_tempValue)
{
	/* initialize the error status */
	ERR_STATUS readValueErr = ERR_SUCCESS;
	if(UNINITIALIZED == genum_tempSensorState)
	{
		readValueErr = ERR_NOTINITIALIZED;
	}
	else
	{
		/* get the converted value only if converting complete */
		if(gu8_adcCompleteFlag == CONVER_NOT_COMPLETE)
		{
			*u8_tempValue = 0;
			readValueErr = ERR_NOT_FINISHED;
		}
		else
		{
			*u8_tempValue = u16_adcConvData;
			gu8_adcCompleteFlag = CONVER_NOT_COMPLETE;

		}
	}

	return readValueErr + TEMP_SENSOR_BASE;
}

/************************************************************************/
/*		             Static Functions Implementation 	        	    */
/************************************************************************/

/*
 * Description: function call Adc_getConvertedValue. function to read the
 * ADCH and ADCL values which contain the converted digital data.
 * */
static void TempSensor_adcCompleteConv_CBK(void)
{
	/* get the ADCH and ADCL values as converting is complete */
	Adc_getConvertedValue(&u16_adcConvData);
	/* indication for readValue function to read the u16_adcConvData variable*/
	gu8_adcCompleteFlag = CONVER_COMPLETE;
}
