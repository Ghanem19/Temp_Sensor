/*
 * temp_sensor.h
 *
 *  Created on: Jan 26, 2020
 *      Author: Ghanem
 */

#ifndef APP_TEMPSENSOR_H_
#define APP_TEMPSENSOR_H_
/************************************************************************/
/*		                        Includes                 		        */
/************************************************************************/
#include "std_types.h"


/************************************************************************/
/*		             External Functions  PROTOTYPES       		        */
/************************************************************************/

/*
 * Function: TempSensor_init.
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: initialize the ADC module to interface with LM34/35 sensor.
 */
extern ERR_STATUS	TempSensor_init(void);

/*
 * Function: TempSensor_convTrigger.
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: trigger the ADC to start converting the analog data.
 */
extern ERR_STATUS	TempSensor_convTrigger(void);

/*
 * Function: TempSensor_readValue.
 *
 * OUTPUTS: *u8_tempValue -> data returned to user by that pointer
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: get the converted digital value when the conversion complete .
 */
extern ERR_STATUS	TempSensor_readValue(uint16 *u8_tempValue);



#endif /* APP_TEMPSENSOR_H_ */
