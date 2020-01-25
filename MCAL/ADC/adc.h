/*
 * adc.h
 *
 *  Created on: Jan 25, 2020
 *      Author: eng-m
 */

#ifndef MCAL_ADC_ADC_H_
#define MCAL_ADC_ADC_H_
/*
typedef uint8 enum_adcInterruptMode_t;
typedef uint8 enum_adcDataAdjut_t;
typedef uint8 enum_adcVref_t;
typedef uint8 enum_adcPrescaler_t;
typedef uint8 enum_adcChannel_t;
*/
#include "std_types.h"


#define ADC_POLLING 		(0U)
#define ADC_INTERRUPT 		(1U) /*adcsra*/


#define ADC_RIGHT_ADGJUST 	(0U)
#define ADC_LEFT_ADGJUST 	(1U)

#define ADC_VREF_INT		(0U)
#define ADC_VREF_VCC		(1U)
#define ADC_VREF_EXT		(2U)

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

typedef struct
{
	uint8 u8_channelNo;
	uint8 u8_refVoltage;
	uint8 u8_dataAdjust;
	uint8 u8_prescaler;
	uint8 u8_resolution;
	uint8 u8_interruptMode;
	uint8 u8_autoTriggSource;/* single, free running or auto Trigger (set the required trigger )*/
	ptr_Func_t ptr_ConvEnd_CBK;
}str_adcCfg_t;

ERR_STATUS Adc_Init(str_adcCfg_t *str_adcCfg);
void Adc_Enable(void);
ERR_STATUS Adc_StartConversion(uint8 u8_channelNumber);
ERR_STATUS Adc_getConvertedValue(uint16 *pu16_convertedData);
void Adc_Disable(void);

#endif /* MCAL_ADC_ADC_H_ */
