/*
 * lcd.c
 *
 *  Created on: Jan 22, 2020
 *      Author: Ghanem
 */
/************************************************************************/
/*		                        Includes                 		        */
/************************************************************************/
#include "common_retval.h"
#include "softwareDelay.h"
#include "Dio.h"
#include "lcd.h"
#include "lcd_cfg.h"
/************************************************************************/
/*		                        Defines                 		        */
/************************************************************************/
#define STEP_1     (enum_functionStep_t) (1U)
#define STEP_2     (enum_functionStep_t) (2U)
#define STEP_3     (enum_functionStep_t) (3U)
#define STEP_4     (enum_functionStep_t) (4U)
#define STEP_5     (enum_functionStep_t) (5U)
#define STEP_6     (enum_functionStep_t) (6U)
#define LAST_STEP  (enum_functionStep_t) (0U)

#define FOUR		(4U)
/************************************************************************/
/*		                     Implicit Types               		        */
/************************************************************************/
typedef uint8 enum_functionStep_t;

/************************************************************************/
/*		                    global variables               		        */
/************************************************************************/
static enum_initState_t genum_lcdInitState = UNINITIALIZED;

/************************************************************************/
/*		            External Functions' Implementation     		        */
/************************************************************************/

/*
 * Function:LCD_init.
 *
 * Return: 	ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: function is initiate the LCD pins to be output either data or control pins. and start send
 * sequence of commands depending on the needed configuration i.e {8-bits or 4 bits, dots matrix 5*7 or 5*10}
 * there are two modes of operation:
 * 1.OS system: at each time LCD_init() called one of the commands would be
 * send until all of commands are ended then the task would be deleted.
 * 2.Super Loop: at this case the delays are allowed so commands could be send after each other directly.
 */
ERR_STATUS LCD_init(void)
{
	/* initiate the error status */
	ERR_STATUS lcdInitErr = ERR_SUCCESS;
	/*declare the DIO structure to be used in OS mode one type*/
	DIO_Cfg_s str_lcdDio;
	if(genum_lcdInitState == INITIALIZED)
	{
		lcdInitErr = ERR_ALREADY_INITIALIZED;

	}
	else
	{
#if (SYSTEM_TYPE == OS)
	/* for OS mode
	 * initiate the state as static variable to detect the previous state of function.*/
		static enum_functionStep_t enum_lcdInitSteps = STEP_1;
		switch(enum_lcdInitSteps)
		{
			case(STEP_1):
			/* initialize LCD pins as outputs */
			str_lcdDio.GPIO = LCD_PORT;
			str_lcdDio.pins = LCD_PINS;
			str_lcdDio.dir = OUTPUT;
			if(E_NOK == DIO_init(&str_lcdDio))
			{
				lcdInitErr = ERR_DIO;
			}
			else
			{
				/* for 4-bit mode should send 3 three times so we send 0x33 then 0x32 */
				lcdInitErr = LCD_sendCommand(LCD_FOUR_BITS);
				if((ERR_NOT_FINISHED + LCD_BASE) == lcdInitErr)
				{
					/* move to next step only if the current 4-bit have been send successfully */
					enum_lcdInitSteps = STEP_2;

				}
				else
				{
					/*Do nothing*/
				}
			}
					break;

 			case(STEP_2):

 				/* send the rest of the previous command */
				lcdInitErr = LCD_sendCommand(LCD_FOUR_BITS);
				if((ERR_SUCCESS + LCD_BASE) == lcdInitErr)
				{
					/* start send next command only if the previous one have been send successfully */
					lcdInitErr = LCD_sendCommand(LCD_FOUR_BITS_CONFIRM);
					if((ERR_NOT_FINISHED + LCD_BASE) == lcdInitErr)
					{
						/* move to next step only if the current 4-bit of command have been send successfully */
						enum_lcdInitSteps = STEP_3;
					}
					else
					{
						/*Do nothing*/
					}
				}
				else
				{
					/*Do nothing*/
				}
					break;

			case(STEP_3):
 		 		/* send the rest of the previous command */
				lcdInitErr = LCD_sendCommand(LCD_FOUR_BITS_CONFIRM);
				if((ERR_SUCCESS + LCD_BASE) == lcdInitErr)
				{
					/* here the LCD is configured as 4-bits
					 * start to set the required specification like dots mattix and line duty */
					lcdInitErr = LCD_sendCommand(TWO_LINES_5x7_DOTS); 	/* 4-bit | 5*7 dots | 1/16 duty*/
					if((ERR_NOT_FINISHED + LCD_BASE) == lcdInitErr)
					{
						/* move to next step only if the current 4-bit of command have been send successfully */
						enum_lcdInitSteps = STEP_4;
					}
					else
					{
						/*Do nothing*/
					}
				}
				else
				{
					/*Do nothing*/
				}
					break;

			case(STEP_4):
 		 		/* send the rest of the previous command */
				lcdInitErr = LCD_sendCommand(TWO_LINES_5x7_DOTS);
				if((ERR_SUCCESS + LCD_BASE) == lcdInitErr)
				{
					/* power the display on after all the configuration parameters have been set*/
					lcdInitErr = LCD_sendCommand(DISPLAY_ON);
					if((ERR_NOT_FINISHED + LCD_BASE) == lcdInitErr)
					{
						/* move to next step only if the current 4-bit of command have been send successfully */
						enum_lcdInitSteps = STEP_5;
					}
					else
					{
						/* Do nothing*/
					}
				}
				else
				{
					/* Do nothing*/
				}
					break;

			case(STEP_5):
 		 		/* send the rest of the previous command */
				lcdInitErr = LCD_sendCommand(DISPLAY_ON);
				if((ERR_SUCCESS + LCD_BASE) == lcdInitErr)
				{
					/* last command is clear the LCD */
					lcdInitErr = LCD_sendCommand(LCD_CLEAR);
					if((ERR_NOT_FINISHED + LCD_BASE) == lcdInitErr)
					{
						/* move to next step only if the current 4-bit of command have been send successfully */
						enum_lcdInitSteps = STEP_6;

					}
					else
					{
						/* do nothing */
					}
				}
				else
				{
					/* do nothing */
				}
					break;
			case(STEP_6):
 		 		/* send the rest of the previous command */
				lcdInitErr = LCD_sendCommand(LCD_CLEAR);
				/* the task suppose to be deleted after this step also to avoid execution of any command
				 * if function called again*/
				enum_lcdInitSteps = LAST_STEP;
				/* indicate to all api's that LCD is initialized successfully */
				genum_lcdInitState = INITIALIZED;
					break;

			default:
				lcdInitErr = ERR_LOGICAL;
				break;
		}

#else
		/* initialize LCD pins as outputs */
		str_lcdDio.GPIO = LCD_PORT;
		str_lcdDio.pins = LCD_PINS;
		str_lcdDio.dir = OUTPUT;

		if(E_NOK == DIO_init(&str_lcdDio))
		{
			lcdInitErr = ERR_DIO;
		}
		else
		{
			/* for 4-bit mode should send 3 three times so we send 0x33 then 0x32 */
			lcdInitErr = LCD_sendCommand(LCD_FOUR_BITS);
			if((ERR_SUCCESS + LCD_BASE) == lcdInitErr)
			{
				lcdInitErr = LCD_sendCommand(LCD_FOUR_BITS_CONFIRM);
				if((ERR_SUCCESS + LCD_BASE) == lcdInitErr)
				{
					/* here the LCD is configured as 4-bits
					 * start to set the required specification like dots matrix and line duty */
					lcdInitErr = LCD_sendCommand(TWO_LINES_5x7_DOTS);  /* 4-bit | 5*7 dots | 1/16 duty*/
					if((ERR_SUCCESS + LCD_BASE) == lcdInitErr)
					{
						/* power the display on after all the configuration parameters have been set*/
						lcdInitErr = LCD_sendCommand(DISPLAY_ON);
						if((ERR_SUCCESS + LCD_BASE) == lcdInitErr)
						{
							/* last command is clear the LCD */
							lcdInitErr= LCD_sendCommand(LCD_CLEAR);
							/* indicate to all api's that LCD is initialized successfully */
							genum_lcdInitState = INITIALIZED;

						}
						else
						{
							/* Do Nothing*/
						}
					}
					else
					{
						/* Do Nothing*/
					}
				}
				else
				{
					/* Do Nothing*/
				}
			}
			else
			{
				/* Do Nothing*/
			}
		}
#endif

	}

	return lcdInitErr;
}

/*
 * Function:LCD_sendCommand.
 *
 * Inputs: u8_command -> represent the required command to be send to LCD.
 *
 * Return: 	ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: function send the required command to LCD i.e (CLEAR, go to certain position...)
 * for each system mode function has a different operation.LCD need processing delay between
 * the upper nibble and lower one so we divide the function to two parts
 * for OS mode -> call command function two times to send the whole command.
 * 2.Super Loop: at this case we just add delay between the first nibble and lower one.
 */
ERR_STATUS LCD_sendCommand(uint8 u8_command)
{
	/* initialize err status */
	ERR_STATUS lcdsendCommandErr = ERR_SUCCESS;
	/* static enum variable to execute only one of the function parts at OS mode */
	static enum_functionStep_t enum_commandFuncStep = STEP_1;
#if (SYSTEM_TYPE == OS)
		switch(enum_commandFuncStep)
		{
			case(STEP_1):
				/* at this part of function select the command register and send the upper nibble*/
				if(E_NOK == DIO_Write(LCD_RS_PORT, LCD_RS_PIN, LOW))
				{
					lcdsendCommandErr = ERR_DIO;
				}
				else
				{
					/* after select sending command */
					/* write high on enable pin enable the to make high-to-low pulse */
					if(E_NOK == DIO_Write(LCD_EN_PORT, LCD_EN_PIN, HIGH))
					{
						lcdsendCommandErr = ERR_DIO;
					}
					else
					{	/* write the higher nibble of the command */
						if(E_NOK == DIO_Write(LCD_PORT,UPPER_NIBBLE,u8_command))
						{
							lcdsendCommandErr = ERR_DIO;
						}
						else
						{
							/* return enable to low to allow LCD to latch the values from buffer */
							if(E_NOK ==	DIO_Write(LCD_EN_PORT, LCD_EN_PIN, LOW))
							{
								lcdsendCommandErr = ERR_DIO;
							}
							else
							{
								/* move to next step at the next execution */
								enum_commandFuncStep = STEP_2;
								lcdsendCommandErr = ERR_NOT_FINISHED;

							}
						}
					}
				}
				break;

			case(STEP_2):
				/* at this part of function send the lower nibble of command*/
					/* write high on enable pin enable the to make high-to-low pulse */
				if(E_NOK == DIO_Write(LCD_EN_PORT, LCD_EN_PIN, HIGH))
				{
					lcdsendCommandErr = ERR_DIO;
				}
				else
				{
					/* send the lower nibble */
					if(E_NOK == DIO_Write(LCD_PORT,UPPER_NIBBLE,u8_command<<FOUR))
					{
						lcdsendCommandErr = ERR_DIO;
					}
					else
					{
						/* return enable to low to allow LCD to latch the values from buffer */
						if(E_NOK == DIO_Write(LCD_EN_PORT, LCD_EN_PIN, LOW))
						{
							lcdsendCommandErr = ERR_DIO;
						}
						else
						{
							/* set the step indicator to it's initial state for the next execution */
							enum_commandFuncStep = STEP_1;
							/* indicate to user that function have been ended */
							lcdsendCommandErr = ERR_SUCCESS;

						}
					}
				}
				break;

			default:
				/* code path doesn't lead to default case except if there is an undefined logic error */
				lcdsendCommandErr = ERR_LOGICAL;
				break;
		}

	#else
		/* select the command register by send logic 0 at RS bit */
		if(E_NOK == DIO_Write(LCD_RS_PORT, LCD_RS_PIN, LOW))
		{
			lcdsendCommandErr = ERR_DIO;
		}
		else
		{
			/* write high on enable pin enable the to make high-to-low pulse */
			if(E_NOK == DIO_Write(LCD_EN_PORT, LCD_EN_PIN, HIGH))
			{
				lcdsendCommandErr = ERR_DIO;
			}
			else
			{	/* write the higher nibble of the command */
				if(E_NOK == DIO_Write(LCD_PORT,UPPER_NIBBLE,u8_command))
				{
					lcdsendCommandErr = ERR_DIO;
				}
				else
				{
					/* return enable to low to allow LCD to latch the values from buffer */
					if(E_NOK ==	DIO_Write(LCD_EN_PORT, LCD_EN_PIN, LOW))
					{
						lcdsendCommandErr = ERR_DIO;
					}
					else
					{
						SwDelay_ms(1);
					}
				}
			}
		}
		if(lcdsendCommandErr == ERR_SUCCESS)
		{
			/* write high on enable pin enable the to make high-to-low pulse */
			if(E_NOK == DIO_Write(LCD_EN_PORT, LCD_EN_PIN, HIGH))
			{
				lcdsendCommandErr = ERR_DIO;
			}
			else
			{
				/* send the lower nibble */
				if(E_NOK == DIO_Write(LCD_PORT,UPPER_NIBBLE,u8_command<<FOUR))
				{
					lcdsendCommandErr = ERR_DIO;
				}
				else
				{
					/* return enable to low to allow LCD to latch the values from buffer */
					if(E_NOK == DIO_Write(LCD_EN_PORT, LCD_EN_PIN, LOW))
					{
						lcdsendCommandErr = ERR_DIO;
					}
					else
					{
						/* Do Nothing*/
					}
				}
			}
		}

	#endif

	return lcdsendCommandErr + LCD_BASE;
}

/*
 * Function:LCD_displayCharacter.
 *
 * Inputs: u8_data -> represent the data to be displayed on LCD.
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: function send the required data by dividing it on two nibbles.
 */
ERR_STATUS LCD_displayCharacter(uint8 u8_data)
{
	ERR_STATUS lcdDisplayCharERR = ERR_SUCCESS;
	/* check if LCD is initialized to start send the data */
	if(UNINITIALIZED == genum_lcdInitState)
	{
		lcdDisplayCharERR  = ERR_NOTINITIALIZED;
	}
	else
	{
		/* select the data register on the LCD */
		if(E_NOK == DIO_Write(LCD_RS_PORT,LCD_RS_PIN, HIGH))
		{
			lcdDisplayCharERR = ERR_DIO;
		}
		else
		{
			/* write high on enable pin enable the to make high-to-low pulse */
			if(E_NOK == DIO_Write(LCD_EN_PORT, LCD_EN_PIN, HIGH))
			{
				lcdDisplayCharERR = ERR_DIO;
			}
			else
			{
				/* send the higher nibble of data*/
				if(E_NOK == DIO_Write(GPIOA,UPPER_NIBBLE,u8_data))
				{
					lcdDisplayCharERR = ERR_DIO;
				}
				else
				{
					/* return enable to low to allow LCD to latch the values from buffer */
					if(E_NOK == DIO_Write(LCD_EN_PORT, LCD_EN_PIN, LOW))
					{
						lcdDisplayCharERR = ERR_DIO;
					}
					else
					{
						/* Do NOTHING */
					}
				}
			}
		}

	}

	/* enter this block if only first one have been done without error */
	if(lcdDisplayCharERR == ERR_SUCCESS)
	{
		/* write high on enable pin enable the to make high-to-low pulse */
		if(E_NOK == DIO_Write(LCD_EN_PORT, LCD_EN_PIN, HIGH))
		{
			lcdDisplayCharERR = ERR_DIO;
		}
		else
		{
			/* send the lower nibble of data*/
			if(E_NOK == DIO_Write(GPIOA, UPPER_NIBBLE, u8_data<<FOUR))
			{
				lcdDisplayCharERR = ERR_DIO;
			}
			else
			{
				/* return enable to low to allow LCD to latch the values from buffer */
				if(E_NOK == DIO_Write(LCD_EN_PORT, LCD_EN_PIN, LOW))
				{
					lcdDisplayCharERR = ERR_DIO;
				}
				else
				{
					/* Do Nothing*/
				}
			}
		}
	}
	else
	{
		/* there was an error in the first block of function */
		/* Do Nothing*/
	}

	return lcdDisplayCharERR + LCD_BASE;
}

/*
 * Function:LCD_displayString.
 *
 * Inputs: au8_string -> array of characters to be displayed on LCD.
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: function has two approach:
 * OS mode	-> at each time function called just send one byte until string is end then delete the task
 * Super loop -> block on the string until all characters is sent.
 */
ERR_STATUS LCD_displayString(uint8 au8_string[])
{
	/* initialize the error status */
	ERR_STATUS lcdDisplayStringErr = ERR_SUCCESS;
	static uint16 su16_dataIndex = 0;
	/* check initialization before start sending */

	if(UNINITIALIZED == genum_lcdInitState)
	{
		lcdDisplayStringErr = ERR_NOTINITIALIZED;
	}
	else
	{

#if(SYSTEM_TYPE == OS)
		/* for each time the function is called send the next byte which
		 * indicated by static index */
		if(au8_string[su16_dataIndex])
		{
			lcdDisplayStringErr = LCD_displayCharacter(au8_string[su16_dataIndex]);
			if((ERR_SUCCESS + LCD_BASE) == lcdDisplayStringErr)
			{
				/* increment the index only if the current byte have transmitted successfully */
				su16_dataIndex++;
				/* indicate to task (caller ) that string display not finished yet*/
				lcdDisplayStringErr = ERR_NOT_FINISHED;
			}
		}
		else
		{
			/* whole sting is transmitted  */
			su16_dataIndex = 0;
			lcdDisplayStringErr = ERR_SUCCESS;
		}
	#else

		while(au8_string[su16_dataIndex])
		{	/* loop one the string untill it's transmitted */
			lcdDisplayStringErr = LCD_displayCharacter(au8_string[su16_dataIndex]);
			if((ERR_SUCCESS + LCD_BASE) == lcdDisplayStringErr)
			{
				/* increment the index only if the current byte have transmitted successfully */
				su16_dataIndex++;
			}
			else
			{
				/* Do Nothing */
			}
		}
		/* return to initial state as it's static variable */
		su16_dataIndex = 0;
	#endif

	}
	return lcdDisplayStringErr + LCD_BASE;
}



/*
 * Function:LCD_gotoRowColumn.
 *
 * Inputs: u8_rows -> the line to move the cursor on it (ROW_0, ROW_1).
 *		   u8_column -> represent the column at the selected row.
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: function move the cursor to the required position.
 * OS mode	-> just need to be called twice to send the command successfully
 * Super loop -> delay on command function would handle it on one time.
 */
ERR_STATUS LCD_gotoRowColumn(uint8 u8_row,uint8 u8_column)
{
	ERR_STATUS lcdGotoRowColErr = ERR_SUCCESS;
	/* check for initialization */
	if(UNINITIALIZED == genum_lcdInitState)
	{

		lcdGotoRowColErr = ERR_NOTINITIALIZED;
	}
	else
	{
#if(SYSTEM_TYPE == OS)

	static enum_functionStep_t enum_gotoXYFuncStep = STEP_1;
	static uint8  u8_address = 0;
	switch(enum_gotoXYFuncStep)
	{
		/*at first step calculate the address and send the upper nibble of the command.*/
		case(STEP_1):
		switch(u8_row)
			{
				/* just add the row offset to the column to get the address of required position */
				case ROW0:
					u8_address = u8_column + ROW0_OFFSET;
					break;
				case ROW1:
					u8_address = u8_column + ROW1_OFFSET;
					break;
				case ROW2:
					u8_address = u8_column + ROW2_OFFSET;
					break;
				case ROW3:
					u8_address = u8_column + ROW3_OFFSET;
					break;
				default:
					lcdGotoRowColErr = ERR_INVALID_ARGU;
					break;
			}
			/*send the upper nibble of the command first*/
			lcdGotoRowColErr = LCD_sendCommand(u8_address | DDRAM_OFFSET);
			enum_gotoXYFuncStep = STEP_2;
				break;

		/*at the second step send the rest of the command */
		case(STEP_2):
		    lcdGotoRowColErr = LCD_sendCommand(u8_address | DDRAM_OFFSET);
			enum_gotoXYFuncStep = STEP_1;
				break;

		default:
			lcdGotoRowColErr = ERR_LOGICAL;
				break;
	}

#else
	uint8  u8_address = 0;
	switch(u8_row)
	{
	/* just add the row offset to the column to get the address*/
		case ROW0:
			u8_address = u8_column + ROW0_OFFSET;
			break;
		case ROW1:
			u8_address = u8_column + ROW1_OFFSET;
			break;
		case ROW2:
			u8_address = u8_column + ROW2_OFFSET;
			break;
		case ROW3:
			u8_address = u8_column + ROW3_OFFSET;
			break;
		default:
			lcdGotoRowColErr = ERR_INVALID_ARGU;
			break;
	}
	/* send the suitable command after calculate the address */
	lcdGotoRowColErr = LCD_sendCommand(u8_address | DDRAM_OFFSET);
#endif

	}

	return lcdGotoRowColErr;
}


/*
 * Function:LCD_sendString_RowCol.
 *
 * Inputs: au8_string -> array of characters to be displayed on LCD.
 *		   u8_rows -> the line to move the cursor on it (ROW_0, ROW_1).
 *		   u8_column -> represent the column at the selected row.
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: function has two approach:
 * OS mode	-> call  LCD_gotoRowColumn() function until sending command is finished,
 *  then just send one byte each time using LCD_displayString() function until string is end then delete the task
 *
 * Super loop -> delay on sending command would handle LCD_gotoRowColumn() function,
 * then block on the string until all characters is sent.
 */
ERR_STATUS LCD_sendString_RowCol(uint8 u8_row,uint8 u8_column,uint8 au8_string[])
{
	ERR_STATUS sendStringXYErr = ERR_SUCCESS;
	/* check if LCD is initialized or not */
	if(UNINITIALIZED == genum_lcdInitState)
	{
		sendStringXYErr = ERR_NOTINITIALIZED;
	}
	else
	{
#if(SYSTEM_TYPE == OS)
		static enum_functionStep_t enum_sendStrToXYStep = STEP_1;
		switch(enum_sendStrToXYStep)
		{
			/* at this step just go on the required position. that's would take two cycles */
			case(STEP_1):
				sendStringXYErr = LCD_gotoRowColumn(u8_row,u8_column);
				if((ERR_SUCCESS + LCD_BASE) == sendStringXYErr)
				{
					/* after move cursor to required position is done successfully move to second step */
					enum_sendStrToXYStep = STEP_2;
					sendStringXYErr = ERR_NOT_FINISHED + LCD_BASE;
				}
				else
				{
					/* Do Nothing */
				}
				break;
			case(STEP_2):
				/*	at this step send the string and LCD_displayString() function would retern not finished until
				 * it's send all the characters */
				sendStringXYErr = LCD_displayString(au8_string);
				if((ERR_SUCCESS + LCD_BASE) == sendStringXYErr)
				{
					enum_sendStrToXYStep = STEP_1;
				}
				else
				{
					/* Do Nothing */
				}
				break;
			default:
				sendStringXYErr = ERR_LOGICAL;
				break;
		}
#else
	/* move cursor to the required position.*/
	sendStringXYErr = LCD_gotoRowColumn(u8_row,u8_column);
	if((ERR_SUCCESS + LCD_BASE) == sendStringXYErr)
	{
		/* display the string starting from this position */
		sendStringXYErr = LCD_displayString(au8_string);
	}
	else
	{
		/* Do Nothing */
	}
#endif
	}
	return sendStringXYErr;
}
