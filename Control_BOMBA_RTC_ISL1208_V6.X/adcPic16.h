/* 
 * File:   adcPic16.h
 * Author: fernando
 *
 * Created on 19 de septiembre de 2014, 16:03
 */

#ifndef ADCPIC16_H
#define	ADCPIC16_H

#define MIDECORRIENTE 0
#define MIDETENSION 1

void setADCChannel(char canal);
char getADCChannel(void);
void openADC(void);

unsigned int adcRead(void);

void startADC(void);
void interruptADC_on(void);
void interruptADC_off(void);
#endif	/* ADCPIC16_H */

