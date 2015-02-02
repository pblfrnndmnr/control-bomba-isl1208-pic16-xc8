#ifndef PWM_H
#define PWM_H
/* 
 * File:   PWM.h
 * Author: fernando
 *
 * Created on 16 de julio de 2014, 22:32
 */
void set_periodo_buzzer(void);
void set_duty(unsigned int duty);
void buzzer_on(unsigned char tiempo);
void buzzer_off(void);

#endif