/*
 * adc.h
 *
 *  Created on: Jan 25, 2020
 *      Author: Ghanem
 */

#ifndef MCAL_ADC_ADC_H_
#define MCAL_ADC_ADC_H_
/************************************************************************/
/*		                        Includes                 		        */
/************************************************************************/
#include "std_types.h"
/************************************************************************/
/*		                        Defines                 		        */
/************************************************************************/
#define ADC_POLLING 			(0U)
#define ADC_INTERRUPT 			(1U)


#define ADC_RIGHT_ADGJUST 		(0U)
#define ADC_LEFT_ADGJUST 		(1U)

#define ADC_VREF_INT			(0U)
#define ADC_VREF_VCC			(1U)
#define ADC_VREF_EXT		    (2U)

#define ADC_PRESCALER_2 		(0x01)
#define ADC_PRESCALER_4			(0x02)
#define ADC_PRESCALER_8			(0x03)
#define ADC_PRESCALER_16		(0x04)
#define ADC_PRESCALER_32		(0x05)
#define ADC_PRESCALER_64		(0x06)
#define ADC_PRESCALER_128		(0x07)

#define ADC_RESOLUTION_8		(0U)
#define ADC_RESOLUTION_10		(1U)

#define ADC_CHANNEL_0			(0x00)
#define ADC_CHANNEL_1			(0x01)
#define ADC_CHANNEL_2			(0x02)
#define ADC_CHANNEL_3			(0x03)
#define ADC_CHANNEL_4			(0x04)
#define ADC_CHANNEL_5			(0x05)
#define ADC_CHANNEL_6			(0x06)
#define ADC_CHANNEL_7			(0x07)

#define ADC_SINGLE_CONV			(0x00)
#define ADC_AUTO_TRIGG_CONV		(0x02)

#define ADC_FREE_RUNNING		(1U)
#define ADC_ANALOG_COMP_TRIGG	(2U)
#define ADC_EXT_INT0_TRIGG		(3U)
#define ADC_TIMER0_COMP_TRIGG	(4U)
#define ADC_TIMER0_OV_TRIGG		(5U)
#define ADC_TIMER1_COMPB_TRIGG	(6U)
#define ADC_TIMER1_OV_TRIGG		(7U)
#define ADC_TIMER1_ICP_TRIGG	(8U)

/************************************************************************/
/*		                     Explicit Types                		        */
/************************************************************************/
typedef struct
{
	uint8 u8_refVoltage;
	uint8 u8_dataAdjust;
	uint8 u8_prescaler;
	uint8 u8_resolution;
	uint8 u8_interruptMode;
	uint8 u8_autoTriggSource;/* single, free running or auto Trigger (set the required trigger )*/
	ptr_Func_t ptr_ConvEnd_CBK;
}str_adcCfg_t;
/************************************************************************/
/*		             External Functions  PROTOTYPES       		        */
/************************************************************************/
/*
 * Function: Adc_init.
 * Input: Pointer to a structure contains the needed information to initialize the ADC.
 * Output:
 * In/Out:
 * Return: ERROR_Status each error code represent certain error to ease debugging.
 * Description: Initiates the ADC module depending on the passed configuration structure.
 */
extern ERR_STATUS Adc_Init(str_adcCfg_t *str_adcCfg);

/*
 * Function: Adc_StartConversion.
 * Input: u8_channelNumber -> channel number to set mux selector bits for specified channel.
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging.
 *
 * Description: trigger the ADC module to start converting the analog data to digital.
 */
extern ERR_STATUS Adc_StartConversion(uint8 u8_channelNumber);

/*
 * Function: Adc_getConvertedValue.
 * Input: *pu16_convertedData -> pointer to store the data in it.
 * Output:
 * In/Out:
 * Return: ERROR_Status each error code represent certain error to ease debugging.
 * Description: read the data from the ADCL and ADCH registers depending on the resolution and adjusment.
 */
extern ERR_STATUS Adc_getConvertedValue(uint16 *pu16_convertedData);

/*
 * Function: Adc_deinit.
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging.
 *
 * Description: uninitialize the ADC as return the used register to their initial value also the used global variables.
 */
extern ERR_STATUS Adc_deinit(void);


#endif /* MCAL_ADC_ADC_H_ */
