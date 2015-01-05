

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif
#include "usart1.h"
#include "system.h"

void configura_USART (void){
 /*En este pic si se habilita el USART los dos pines(RX y TX) quedan afectados a
    este periférico y no pueden ser usados como I/O independientes*/
    //Calculo la carga del registro en base al baudrate elegido baudrate
//FOSC=_XTAL_FREQ
//X=SPBRG
//BAUDRATE=FOSC/(64(X+1)) //en low speed
//BAUDRATE=FOSC/(16(X+1)) //en high speed
//X=FOSC/64/BAUDRATE-1 en low speed
//X=FOSC/16/BAUDRATE-1 en low speed
 //Si FOSC=20MHZ
//X=20000000/64/2400-1

//X=129 para low speed
SPBRG= _XTAL_FREQ/64/baudrate-1;
//SPBRG=129;
    /*TXSTAbits.CSRC=1; Asynchronous mode
Don?t care
Synchronous mode
1 = Master mode (Clock generated internally from BRG)
0 = Slave mode (Clock from external source)*/

/*TXSTAbits.TX9=0;9-bit Transmit Enable bit
        1 = Selects 9-bit transmission
        0 = Selects 8-bit transmission*/
RCSTAbits.RX9=0; /*9-bit Transmit Enable bit*/
TXSTAbits.SYNC=0;/* USART Mode Select bit
1 = Synchronous mode
0 = Asynchronous mode*/


TXSTAbits.BRGH=0; /*High Baud Rate Select bit
Asynchronous mode
1 = High speed
0 = Low speed
Synchronous mode
Unused in this mode*/
//TXSTAbits.TX9D=1;// 9th bit of transmit data. Can be parity bit.
//Note: SREN/CREN overrides TXEN in SYNC mode.
 

TXSTAbits.TXEN=1; /*Transmit Enable bit(1)
1 = Transmit enabled
0 = Transmit disabled*/
RCSTAbits.CREN=1;
RCSTAbits.SPEN=1;
}


/*/prototipo de la funcion PUTCH para poder enviar al USART con printf
void putch(char data) {
 while( ! TXIF)
 continue;
 TXREG = data;
}*/
 char getch(void) {
    return RCREG;
}

bit kbhit (void){ //el ide no reconoce __bit pero funciona
     return RCIF;
 }