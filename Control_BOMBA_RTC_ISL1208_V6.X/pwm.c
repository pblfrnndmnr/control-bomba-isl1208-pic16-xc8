#include <xc.h>
#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include "system.h"
#include "pwm.h"
#include "user.h"
/*The following steps should be taken when configuring
the CCP module for PWM operation:
1. Set the PWM period by writing to the PR2 register.
2. Set the PWM duty cycle by writing to the
CCPR1L register and CCP1CON<5:4> bits.
3. Make the CCP1 pin an output by clearing the
TRISC<2> bit.
4. Set the TMR2 prescale value and enable Timer2
by writing to T2CON.
5. Configure the CCP1 module for PWM operation.*/
#define FrecuenciaPWM 4880UL
#define PeriodoPWM 1/FrecuenciaPWM
#define PRESCALERTIMER2 4
#define asignaPR2 _XTAL_FREQ/PRESCALERTIMER2/4*PeriodoPWM-1

void set_periodo_buzzer(void){
//PWM Period = [(PR2) + 1] * 4 * TOSC *(TMR2 Prescale Value)
    //PR2=PWM PERIOD/(4*TOSC*TMR2 Prescale Value)-1
    //para una frecuencia de __XTAL_FREQ/4=5Mhz
    PR2=asignaPR2;//periodo;;//periodo; //asigno el periodo
    T2CONbits.TMR2ON=1; //Habilito el timer2
    T2CONbits.T2CKPS=1; //prescaler en 4
    // CCP1CONbits.CCP1M=0b1100; //Configuro el CCp1 como PWM
    RB2=0;
   TRISC2=0;//pongo el pin RC2/PWM1 como salida
#warning No olvidar que si el PWM_duty > PWM_period no queda bien definida la frecuencia
   set_duty(512);
}

/*/PWM Duty Cycle =(CCPR1L:CCP1CON<5:4>)*TOSC*(TMR2 Prescale Value)*/
void set_duty(unsigned int duty){

#define PWM10Bits
    //pwm modo 10bits
#ifdef PWM10Bits
    CCPR1L=duty>>2;
#endif
CCP1CON = (CCP1CON & 0xCF) | ((duty <<4) & 0x30);



}

void buzzer_off(void){
  //activapor=0;
    CCP1CONbits.CCP1M=0; //desactiva pwm
PORTCbits.RC2=0; //pongo el pin de reloj en bajo

}
void buzzer_on(void){
  activapor=3;
    CCP1CONbits.CCP1M=0b1100; //activa pwm
      }