
/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "user.h"
#include "LCDGeneric.h"
/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void)
{
    /* Initialize User Ports/Peripherals/Project here */

    /* Setup analog functionality and port direction */

    /* Initialize peripherals */

    /* Enable interrupts */
}

//prototipo de la funcion PUTCH para poder enviar al USART  o al LCD con printf
void putch(char data) {
    //la variable envia_por nos indica si el printf envia los datos al lcd o al USART
  if (envia_por==LCD)
   vLCD_Putc(data);
  else{
    while( ! TXIF)
    continue;
    TXREG = data;
    }
}


int aniobisiesto(unsigned char year)
{
    return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}
