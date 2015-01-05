#ifndef	_SYSTEM_H_
#define	_SYSTEM_H_
/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/

/* Define system operating frequency */

/* Microcontroller MIPs (FCY) */
#define _XTAL_FREQ 20000000UL
#define SYS_FREQ        500000L
#define FCY             SYS_FREQ/4
#define PIC_CLK   20000000   // cristal 20MHz
 #define I2C_BAUD  400000     // velocidad 100 KHz

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

/* Custom oscillator configuration funtions, reset source evaluation
functions, and other non-peripheral microcontroller initialization functions
go here. */

void ConfigureOscillator(void); /* Handles clock switching/osc initialization */


#endif