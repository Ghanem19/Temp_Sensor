/*
 * adc.c
 *
 *  Created on: Jan 25, 2020
 *      Author: Ghanem
 */
/************************************************************************/
/*		                        Includes                 		        */
/************************************************************************/
#include "common_retval.h"
#include "common_macros.h"
#include "registers.h"
#include "interrupt.h"
#include "Dio.h"
#include "adc.h"
/************************************************************************/
/*		                        Defines                 		        */
/************************************************************************/
/* ADMUX register bits  */
#define	MUX0    	BIT0
#define	MUX1 		BIT1
#define	MUX2 	  	BIT2
#define	MUX3 		BIT3
#define	MUX4	  	BIT4
#define	ADLAR		BIT5
#define	REFS0	 	BIT6
#define	REFS1		BIT7
/* ADCSRA register bits */
#define ADPS0		BIT0
#define ADPS1		BIT1
#define ADPS2		BIT2
#define ADIE 		BIT3
#define ADIF 		BIT4
#define ADATE		BIT5
#define ADSC 		BIT6
#define ADEN		BIT7

/* SFIOR register used bits */
#define ADTS0		BIT5
#define ADTS1		BIT6
#define ADTS2		BIT7

#define LEFT_ADJUSTMENT_OFFSET (6U)

/* bits group defines to assign value directly on them */
#define VREF_BITS			(REFS0|REFS1)
#define TRIGG_SRC_BITS		(ADTS2 | ADTS1 | ADTS0)
#define PRESCALER_BITS  	(ADPS2 | ADPS1 | ADPS0)
#define CHANNEL_BITS  		(MUX3 | MUX2 | MUX1 | MUX0)
#define ADC_CHANNEL_NUM 8
/************************************************************************/
/*		                     Global Variables	                 	    */
/************************************************************************/
enum_initState_t genum_adcInitState  = UNINITIALIZED;
uint8 gu8_adcResolution = 0;
uint8 gu8_dataAdjustment = 0;
ptr_Func_t pfunc_conversionComplete_CBK = NULL;
/* set the lookup tables*/
const uint8 au8_vrefMaskTable[] = {0x0, 0x40, 0xC0};
const uint8 au8_triggerSrsMaskTable[] = {0x0,0x0,0x20, 0x40, 0x60, 0x80, 0xA0, 0xC0, 0xE0};

/************************************************************************/
/*		                    	ISR's 			                 	    */
/************************************************************************/
ISR(ADC_vect)
{
	/* check for null pointer */
	if(pfunc_conversionComplete_CBK != NULL)
	{
		pfunc_conversionComplete_CBK();
	}
	else
	{
		/* Do nothing */
	}
}


/************************************************************************/
/*		             External Functions Implementation 	        	    */
/************************************************************************/

/*
 * Function: Adc_init.
 * Input: Pointer to a structure contains the needed information to initialize the ADC.
 * Output:
 * In/Out:
 * Return: ERROR_Status each error code represent certain error to ease debugging.
 * Description: Initiates the ADC module depending on the passed configuration structure.
 */
ERR_STATUS Adc_Init(str_adcCfg_t *str_adcCfg)
{
	/* initialize the error status */
	ERR_STATUS adcInitErr = ERR_SUCCESS;

	if(str_adcCfg == NULL)
	{
		adcInitErr = ERR_NULL_PTR;
	}
	else
	{

		/* check if the same channel is initialized before */
		if(genum_adcInitState == INITIALIZED)
		{
			adcInitErr = ERR_ALREADY_INITIALIZED;
		}
		else
		{
			if(str_adcCfg->u8_refVoltage > ADC_VREF_EXT)
			{
				adcInitErr = ERR_INVALID_ARGU;
			}
			else
			{
				/* set the voltage reference if {external, internal ,VCC }*/
				ASSIGN_BIT(ADMUX,VREF_BITS,au8_vrefMaskTable[str_adcCfg->u8_refVoltage]);
				if(str_adcCfg->u8_interruptMode > ADC_INTERRUPT)
				{
					adcInitErr = ERR_INVALID_ARGU;
				}
				else
				{
					/* decide if interrupt or pooling mode */
					ASSIGN_BIT(ADCSRA,ADIE,str_adcCfg->u8_interruptMode*HIGH);
					if(str_adcCfg->u8_interruptMode == ADC_INTERRUPT)
					{
						/* for interrupt mode pass the call back function */
						pfunc_conversionComplete_CBK = str_adcCfg->ptr_ConvEnd_CBK;
					}
					else
					{
						/*Do Nothing*/
					}
					if(str_adcCfg->u8_dataAdjust > ADC_LEFT_ADGJUST)
					{
						adcInitErr = ERR_INVALID_ARGU;
					}
					else
					{
						/* specify if right or left adjusted */
						ASSIGN_BIT(ADMUX, ADLAR, str_adcCfg->u8_dataAdjust*HIGH);
						/* pass the selected adjustment and resolution to global variables to be used in read function */
						gu8_dataAdjustment = str_adcCfg->u8_dataAdjust;
						if(str_adcCfg->u8_resolution > ADC_RESOLUTION_10)
						{
							adcInitErr = ERR_INVALID_ARGU;
						}
						else
						{
							gu8_adcResolution = str_adcCfg->u8_resolution;

						}
					}
				}
			}
		}
	}

	/* insure that the previous block done successfully */
	if(adcInitErr == ERR_SUCCESS)
	{
		/* check if prescaler doesn't exceed the limits */
		if(str_adcCfg->u8_prescaler > ADC_PRESCALER_128)
		{
			adcInitErr = ERR_INVALID_ARGU;
		}
		else
		{
			/* set the required prescaler */
			ASSIGN_BIT(ADCSRA ,PRESCALER_BITS, str_adcCfg->u8_prescaler);
			/* decide if ADC module would convert data repeatedly by triggering event or just one time conversion*/
			if(str_adcCfg->u8_autoTriggSource == ADC_SINGLE_CONV)
			{
				/*for single conversion mode no need to deal with SFIOR */
				CLR_BIT(ADCSRA, ADATE);
				/* just enable the ADC module */
				SET_BIT(ADCSRA,ADEN);
				/* indicate that the module is initialized successfully */
				genum_adcInitState = INITIALIZED;

			}
			else if((str_adcCfg->u8_autoTriggSource >= ADC_FREE_RUNNING) && \
					(str_adcCfg->u8_autoTriggSource <= ADC_TIMER1_ICP_TRIGG))
			{
				/* for this part the trigger action should be initialized externally */
				SET_BIT(ADCSRA, ADATE);
				ASSIGN_BIT(SFIOR,TRIGG_SRC_BITS, au8_triggerSrsMaskTable[str_adcCfg->u8_autoTriggSource]);
				/* just enable the ADC module */
				SET_BIT(ADCSRA,ADEN);
				/* indicate that the module is initialized successfully */
				genum_adcInitState = INITIALIZED;

			}
			else
			{
				adcInitErr = ERR_INVALID_ARGU;
			}

		}
	}

	return adcInitErr + ADC_BASE;
}

/*
 * Function: Adc_StartConversion.
 * Input: u8_channelNumber -> channel number to set mux selector bits for specified channel.
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging.
 *
 * Description: trigger the ADC module to start converting the analog data to digital.
 */
ERR_STATUS Adc_StartConversion(uint8 u8_channelNumber)
{
	/* initialize the error status */
	ERR_STATUS adcStartConvErr = ERR_SUCCESS;
	if(genum_adcInitState == UNINITIALIZED)
	{
		/* not initialize channel to start conversion */
		adcStartConvErr = ERR_NOTINITIALIZED;
	}
	else
	{
		/* check if channel number exceed the maximum number of channels*/
		if(u8_channelNumber >= ADC_CHANNEL_NUM)
		{
			adcStartConvErr = ERR_INVALID_ARGU;
		}
		else
		{
			/* enable the required channel */
			ASSIGN_BIT(ADMUX, CHANNEL_BITS, u8_channelNumber);

			/* set the start conversion pin */
			SET_BIT(ADCSRA,ADSC);
		}

	}

	return adcStartConvErr + ADC_BASE;
}

/*
 * Function: Adc_getConvertedValue.
 * Input: *pu16_convertedData -> pointer to store the data in it.
 * Output:
 * In/Out:
 * Return: ERROR_Status each error code represent certain error to ease debugging.
 * Description: read the data from the ADCL and ADCH registers depending on the resolution and adjusment.
 */
ERR_STATUS Adc_getConvertedValue(uint16 *pu16_convertedData)
{
	/* initialize error status */
	ERR_STATUS getValueErr = ERR_SUCCESS;
	/* check if module is initialized */
	if(UNINITIALIZED == genum_adcInitState)
	{
		getValueErr = ERR_NOTINITIALIZED;
	}
	else
	{
		/* check for null pointer */
		if(NULL == pu16_convertedData)
		{
			getValueErr = ERR_NULL_PTR;
		}
		else
		{
			/* setect how to read the converted data */
			switch(gu8_adcResolution)
			{
				case(ADC_RESOLUTION_8):
					switch(gu8_dataAdjustment)
					{
						case(ADC_RIGHT_ADGJUST):
							/*8-bit resolution and right adjustment*/
							*pu16_convertedData = ADCL;
							break;
						case(ADC_LEFT_ADGJUST):
							/*8-bit resolution and left adjustment*/
							*pu16_convertedData = ADCH;
							break;
						default:
							/* not valid adjustment*/
							getValueErr = ERR_LOGICAL;
					}
					break;

				case(ADC_RESOLUTION_10):
					switch(gu8_dataAdjustment)
					{
						case(ADC_RIGHT_ADGJUST):
							/*10-bit resolution and right adjustment
							 * no need to read ADCL first to lock ADCH it would work properly by 16-bit register*/
							*pu16_convertedData = ADC_DATA;
							break;
						case(ADC_LEFT_ADGJUST):
						/*10-bit resolution and left adjustment*/
						*pu16_convertedData = (ADC_DATA)>>(LEFT_ADJUSTMENT_OFFSET);
							break;
						default:
							/* not valid adjustment*/
							getValueErr = ERR_LOGICAL;
					}
				break;

				default:
					/* not valid resolution */
					getValueErr = ERR_LOGICAL;
					break;
			}
		}
	}

	return getValueErr + ADC_BASE;
}

/*
 * Function: Adc_deinit.
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging.
 *
 * Description: uninitialize the ADC as return the used register to their initial value also the used global variables.
 */
ERR_STATUS Adc_deinit(void)
{
	ERR_STATUS adcDeinitErr = ERR_SUCCESS;
	/* check if the module is initialized */
	if(UNINITIALIZED == genum_adcInitState)
	{
		adcDeinitErr = ERR_NOTINITIALIZED;
	}
	else
	{
		/* clear the used registers */
		ADMUX = 0;
		ADCSRA = 0;
		ASSIGN_BIT(SFIOR,TRIGG_SRC_BITS, LOW);
		/* set all global variables to their initial value */
		gu8_adcResolution = 0;
		gu8_dataAdjustment = 0;
		pfunc_conversionComplete_CBK = NULL;
		genum_adcInitState = UNINITIALIZED;
	}

	return adcDeinitErr + ADC_BASE;
}
