/******************************************************************************
 *
 * 	Module: 		common
 *
 *	File Name: 		common_retval.h
 *
 * 	Description: 	holds common errors returned over modules
 *
 *******************************************************************************/

#ifndef COMMON_RETVAL_H_
#define COMMON_RETVAL_H_

/************************************************************************/
/*		                        Defines                 		        */
/************************************************************************/
/* error types*/


#define ERR_SUCCESS 				    ( 0)
#define ERR_INVALID_ARGU	    	    (-1)
#define ERR_NOTINITIALIZED 				(-2)
#define ERR_ALREADY_INITIALIZED  		(-3)
#define ERR_NULL_PTR					(-4)
#define ERR_RESOURCE_NOT_FOUND 			(-5)
#define ERR_HW_CONSTRAIN				(-6)
#define ERR_BUFFER_FULL					(-7)
#define ERR_BUFFER_LOCKED				(-8)
#define ERR_DIO 						(-9)
#define ERR_TIMER						(-10)
#define ERR_UART						(-11)
#define ERR_SPI 						(-12)
#define ERR_LOGICAL						(-13)
#define ERR_NOT_FINISHED				(-14)
/* modules bases */
#define KEYPAD_BASE                     (-10)
#define LCD_BASE   	                    (-30)
#define TMU_BASE						(-50)
#define BCM_BASE						(-70)
#define SOS_BASE						(-90)

#endif /* COMMON_RETVAL_H_ */
