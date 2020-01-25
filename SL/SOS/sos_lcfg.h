/*
 * TMU_Lcfg.h
 *
 *  Created on: Jan 11, 2020
 *      Author: Ghanem
 */

#ifndef SL_SOS_SOS_LCFG_H_
#define SL_SOS_SOS_LCFG_H_

/************************************************************************/
/*		                        Includes                 		        */
/************************************************************************/
#include "std_types.h"

/************************************************************************/
/*		                     Implicit Types               		        */
/************************************************************************/
typedef struct{

	uint8 u8_timerId;
	uint8 u8_sysTick;

}str_sosCfgParam_t;

/************************************************************************/
/*		                    external Global                		        */
/************************************************************************/
extern str_sosCfgParam_t  gstr_sosConfig;

#endif /* SL_SOS_SOS_LCFG_H_ */
