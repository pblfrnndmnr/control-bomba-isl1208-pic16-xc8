
/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif
#include <stdbool.h>        /* For true/false definition */
#include <stdint.h>
#include "user.h"
#include "system.h"

#include "Pulsadores.h"
#include "pwm.h"
#include "adcPic16.h"
#include "_isl1208.h"
/* Interrupt Routines                                                         */
/******************************************************************************/

/* Baseline devices don't have interrupts. Note that some PIC16's 
 * are baseline devices.  Unfortunately the baseline detection macro is 
 * _PIC12 */
#ifndef _PIC18
#ifndef _PIC12

void interrupt isr(void) {
    if (ADIF && ADIE) {
        if (getADCChannel() == MIDECORRIENTE) {
            medidaI_adc = adcRead();
            setADCChannel(MIDETENSION);
        } else {
            medidaV_adc = adcRead();
            /*TODO debo realizar un promedio en las mediciones para filtrar las
            variaciones rápidas de tensión*/
            setADCChannel(MIDECORRIENTE);
        }
        ADIF = 0;
        cambio_de_canal = 1;
    } else if (INTF && INTE) { // INTERRUPCION por EXT2 Clock Out --------------------------------------------
        if (INTEDG == 1) {
            //      ext_int_edge(2,H_TO_L);
            INTEDG = 0;
            flanco = 0;
            //PORTEbits.RE0 = 1; //output_high(PIN_E0);
        } else {
            //   ext_int_edge(2,L_TO_H);
            INTEDG = 1;
            flanco = 1;
            // PORTEbits.RE0 = 0; //output_low(PIN_E0);
            //      flanco=1;

        }
        refrescadisplay = 1;
        INTF = 0;
    } else if (T0IF && T0IE) {

        //Interrupcion por desborde del timer0. Tal como esta configurado, se el timer desborda cada
        //0.003264seg o sea 3.264 ms, por lo tanto para que pase alrededor de 0.5segundo, se debe entrar 150 veces
        //como con un unsigned int no funciona uso unsigned char y solo hasta 150
        //cada 3ms realizamos una conversion analogica para obtener la medida de corriente
        if (cambio_de_canal)
            cambio_de_canal = 0;
        else
            startADC();
        if (cuenta40ms < 12) {
            cuenta40ms++;
        } else {
            cuenta40ms = 0;
            if (activapor > 0) {
                activapor--;
            } else {
                buzzer_off();
                activapor = 0;
            }
            if (cuenta500ms < 11) {
                cuenta500ms++;


            } else {

                cuenta500ms = 0;
                flanco = !flanco;
                refrescadisplay = 1;
                if (cuentasegundos < 60) {

                    cuentasegundos++;
                } else {
                    //TODO quitar esto en la version final
                    TRISDbits.TRISD3 = 0;
                    PORTDbits.RD3 = !PORTDbits.RD3;
                    //TODO quitar esto en la version final
                    salidabomba = !salidabomba;
                    cuentasegundos = 0;
                    if (cuentaminutos < 60) {
                        cuentaminutos++;
                    } else {
                        cuentaminutos = 0;
                        if (cuentahoras < 24) {
                            cuentahoras++;
                        } else
                            cuentahoras = 0;
                        if (cuentadias < 30) {
                            cuentadias++;
                        } else {
                            cuentadias = 0;
                        }
                    }

                }
            }
        }
        static char maximodia;
        if (Pulsacion(0, BOTON_Subir, CON_REPETICION, LOGICA_INVERSA)) {

            refrescadisplay = 1;
            switch (modificafecha) {
                case MINUTOS:
                    haycambio = 1;
                    if ((*horario).min < 59) (*horario).min++;
                    else
                        if ((*horario).min == 59) (*horario).min = 0;
                    break;
                case HORA:
                    haycambio = 1;
                    if ((*horario).hrs < 23) (*horario).hrs++;
                    else
                        if ((*horario).hrs == 23) (*horario).hrs = 0;
                    break;
                case DIA:
                    haycambio = 1;
                    if (fecha.day < maximodia) fecha.day++;
                    else
                        if (fecha.day == maximodia) fecha.day = 1;
                    break;
                case MES:
                    haycambio = 1;
                    if (fecha.month < 12) fecha.month++;
                    else
                        if (fecha.month == 12) fecha.month = 1;
                    break;
                case ANIO:
                    haycambio = 1;
                    if (fecha.yr < 99) fecha.yr++; //solamente vamos hasta el año 00
                    else
                        if (fecha.yr == 99) fecha.yr = 0; //solamente contamos desde el año 99
                    break;
                case SINO:
                    banderasino = !banderasino;
                    break;
                case TIEMPOENCENDIDO:
                    haycambio = 1;
                    if (tiempoencendido < TIEMPOMAXIMOENCENDIDO) tiempoencendido++; //
                    else
                        if (tiempoencendido == TIEMPOMAXIMOENCENDIDO) tiempoencendido = 0;
                    break;
                case TIEMPOFALLA:
                    haycambio = 1;
                    if (tiempofalla < TIEMPOMAXIMOFALLA) tiempofalla++; //
                    else
                        if (tiempofalla == TIEMPOMAXIMOFALLA) tiempofalla = 0;
                    break;
                case PERIODOENCENDIDO:
                    haycambio = 1;
                    if (periodoencendido < TIEMPOMAXIMOPERIODO) periodoencendido++; //
                    else
                        if (periodoencendido == TIEMPOMAXIMOPERIODO) periodoencendido = 1;
                    break;
                default:
                    break;
            }
            if (aniobisiesto(fecha.yr) && fecha.month == 2) {
                maximodia = 29;
            } else if (fecha.month == 2)
                maximodia = 28;
            if (fecha.month == 4 || fecha.month == 6 || fecha.month == 9 || fecha.month == 11) {
                maximodia = 30;
            }
            if (fecha.month == 1 || fecha.month == 3 || fecha.month == 5 || fecha.month == 7 || fecha.month == 8 || fecha.month == 10 || fecha.month == 12) {
                maximodia = 31;
            }
            if (fecha.day > maximodia)
                fecha.day = maximodia;
        }
        if (Pulsacion(1, BOTON_Bajar, CON_REPETICION, LOGICA_INVERSA)) {

            refrescadisplay = 1;
            switch (modificafecha) {
                case MINUTOS:
                    haycambio = 1;
                    if ((*horario).min > 0) (*horario).min--;
                    else
                        if ((*horario).min == 0) (*horario).min = 59;
                    break;
                case HORA:
                    haycambio = 1;
                    if ((*horario).hrs > 0) (*horario).hrs--;
                    else
                        if ((*horario).hrs == 0) (*horario).hrs = 23;
                    break;
                case DIA:
                    haycambio = 1;
                    if (fecha.day > 1) fecha.day--;
                    else
                        if (fecha.day == 1) fecha.day = maximodia;
                    break;
                case MES:
                    haycambio = 1;
                    if (fecha.month > 1) fecha.month--;
                    else
                        if (fecha.month == 1) fecha.month = 12;
                    break;
                case ANIO:
                    haycambio = 1;
                    if (fecha.yr > 0) fecha.yr--; //solamente vamos hasta el año 00
                    else
                        if (fecha.yr == 0) fecha.yr = 99; //solamente contamos desde el año 99
                    break;
                case SINO:
                    banderasino = !banderasino;
                    break;
                case TIEMPOENCENDIDO:
                    haycambio = 1;
                    if (tiempoencendido > 0) tiempoencendido--; //
                    else
                        if (tiempoencendido == 0) tiempoencendido = TIEMPOMAXIMOENCENDIDO;
                    break;
                case TIEMPOFALLA:
                    haycambio = 1;
                    if (tiempofalla > 0) tiempofalla--; //
                    else
                        if (tiempofalla == 0) tiempofalla = TIEMPOMAXIMOFALLA;
                    break;
                case PERIODOENCENDIDO:
                    haycambio = 1;
                    if (periodoencendido > 1) periodoencendido--; //
                    else
                        if (periodoencendido == 1) periodoencendido = TIEMPOMAXIMOPERIODO;
                    break;
                default:
                    break;
            }
            if (aniobisiesto(fecha.yr) && fecha.month == 2) {
                maximodia = 29;
            } else if (fecha.month == 2)
                maximodia = 28;
            if (fecha.month == 4 || fecha.month == 6 || fecha.month == 9 || fecha.month == 11) {
                maximodia = 30;
            }
            if (fecha.month == 1 || fecha.month == 3 || fecha.month == 5 || fecha.month == 7 || fecha.month == 8 || fecha.month == 10 || fecha.month == 12) {
                maximodia = 31;
            }
            if (fecha.day > maximodia)
                fecha.day = maximodia;
        }


        bandera_startglobal = 0;
        if (Pulsacion(3, BOTON_ONOFF, INMEDIATO, LOGICA_INVERSA)) {

            bandera_startglobal = 1;
        }

        if (Pulsacion(4, BOTON_MENU, SIN_REPETICION/*INMEDIATO*/, LOGICA_INVERSA)) {

            buzzer_on();
            if (menuactual < ULTIMOMENU)
                menuactual++;
            else
                menuactual = 0;
        }
        T0IF = 0;
    } else {
        ///interrupciones no contempladas
    }

    /* This code stub shows general interrupt handling.  Note that these
    conditional statements are not handled within 3 seperate if blocks.
    Do not use a seperate if block for each interrupt flag to avoid run
    time errors. */

#if 0

    /* Add interrupt routine code here. */

    /* Determine which flag generated the interrupt */
    if (<Interrupt Flag 1 >) {
        <Interrupt Flag 1 = 0 >; /* Clear Interrupt Flag 1 */
    } else if (<Interrupt Flag 2 >) {
        <Interrupt Flag 2 = 0 >; /* Clear Interrupt Flag 2 */
    } else {
        /* Unhandled interrupts */
    }

#endif

}
#endif


#endif