#ifndef	_USART_H_
#define	_USART_H_

/* 
 * File:   usart.h
 * Author: fernando
 *
 * Created on 29 de abril de 2014, 22:37
 */
#include "system.h"
#define baudrate 2400

void configura_USART (void);
void putch(char data);
char getch(void) ;
__bit	kbhit(void);



#endif