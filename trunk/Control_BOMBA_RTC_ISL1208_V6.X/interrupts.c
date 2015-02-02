
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

void interrupt isr(void) {
    if (ADIF && ADIE) {
        // <editor-fold defaultstate="collapsed" desc="interrupcion adc">
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
        // </editor-fold>
    } else if (INTF && INTE) { // INTERRUPCION por EXT2 Clock Out --------------------------------------------
        // <editor-fold defaultstate="collapsed" desc="interrupcion intf">
        alarma_encendido = SIALARMA;
        INTF = 0;
        // </editor-fold>
    } else if (T0IF && T0IE) {
        // <editor-fold defaultstate="collapsed" desc="interrupcion timer0">
        //Interrupcion por desborde del timer0. Tal como esta configurado, se el timer desborda cada
        //0.0032768seg o sea 3.2768 ms, por lo tanto para que pase alrededor de 0.5segundo, se debe entrar 150 veces
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
            if (activapwmpor > 0) {
                activapwmpor--;
            } else {
                buzzer_off();
                activapwmpor = 0;
            }
            if (cuenta500ms < 11) {
                cuenta500ms++;


            } else {
                cuenta500ms = 0;
                flanco = !flanco;

                //para que entre a lo que sigue cada 1 segundo aprovecho a leer el valor de flanco
                if (flanco) {
                    if (indica_secuencia_arranque) {
                        if (cuenta_tiempofalla > 0) {
                            cuenta_tiempofalla--;
                        }
                        if (cuenta_tiempofalla == 0) {
                            indica_tiempo_falla = 1;
                        }
                    }
                    if (bandera_orden_on_off_bomba || bandera_orden_Alarma_bomba) {
                        if (tiempo_secuencia_arranque > 0) {
                            tiempo_secuencia_arranque--;
                        }
                        if (tiempo_secuencia_arranque == 0) {
                            indica_secuencia_arranque = 1;
                        }
                    }
                } else {
                    actualizo_datos_rtc = 1;
                }

                refrescadisplay = 1;
                if (cuentasegundos < 60) {

                    cuentasegundos++;
                } else {
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
        // <editor-fold defaultstate="collapsed" desc="boton subir">
        static char maximodia;
        if (Pulsacion(0, BOTON_Subir, CON_REPETICION, LOGICA_INVERSA)) {
            haycambio = 1;
            refrescadisplay = 1;
            switch (modificafecha) {
                case MINUTOS:
                    //haycambio = 1;
                    if ((*horario).min < 59) (*horario).min++;
                    else
                        if ((*horario).min == 59) (*horario).min = 0;
                    break;
                case HORA:
                    //haycambio = 1;
                    if ((*horario).hrs < 23) (*horario).hrs++;
                    else
                        if ((*horario).hrs == 23) (*horario).hrs = 0;
                    break;
                case DIA:
                    //haycambio = 1;
                    if (fecha.day < maximodia) fecha.day++;
                    else
                        if (fecha.day == maximodia) fecha.day = 1;
                    break;
                case MES:
                    //haycambio = 1;
                    if (fecha.month < 12) fecha.month++;
                    else
                        if (fecha.month == 12) fecha.month = 1;
                    break;
                case ANIO:
                    //haycambio = 1;
                    if (fecha.yr < 99) fecha.yr++; //solamente vamos hasta el año 00
                    else
                        if (fecha.yr == 99) fecha.yr = 0; //solamente contamos desde el año 99
                    break;
                case SINO:
                    *banderasino = !*banderasino;
                    break;
                case TIEMPOENCENDIDO:
                    //haycambio = 1;
                    if (tiempoencendido < TIEMPOMAXIMOENCENDIDO) tiempoencendido++; //
                    else
                        if (tiempoencendido == TIEMPOMAXIMOENCENDIDO) tiempoencendido = 0;
                    break;
                case TIEMPOFALLA:
                    //haycambio = 1;
                    if (tiempofalla < TIEMPOMAXIMOFALLA) tiempofalla++; //
                    else
                        if (tiempofalla == TIEMPOMAXIMOFALLA) tiempofalla = 0;
                    break;
                case PERIODOENCENDIDO:
                    //haycambio = 1;
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
        // </editor-fold>
        // <editor-fold defaultstate="collapsed" desc="boton bajar">
        if (Pulsacion(1, BOTON_Bajar, CON_REPETICION, LOGICA_INVERSA)) {
            haycambio = 1;
            refrescadisplay = 1;
            switch (modificafecha) {
                case MINUTOS:
                    //haycambio = 1;
                    if ((*horario).min > 0) (*horario).min--;
                    else
                        if ((*horario).min == 0) (*horario).min = 59;
                    break;
                case HORA:
                    //haycambio = 1;
                    if ((*horario).hrs > 0) (*horario).hrs--;
                    else
                        if ((*horario).hrs == 0) (*horario).hrs = 23;
                    break;
                case DIA:
                    //haycambio = 1;
                    if (fecha.day > 1) fecha.day--;
                    else
                        if (fecha.day == 1) fecha.day = maximodia;
                    break;
                case MES:
                    //haycambio = 1;
                    if (fecha.month > 1) fecha.month--;
                    else
                        if (fecha.month == 1) fecha.month = 12;
                    break;
                case ANIO:
                    //haycambio = 1;
                    if (fecha.yr > 0) fecha.yr--; //solamente vamos hasta el año 00
                    else
                        if (fecha.yr == 0) fecha.yr = 99; //solamente contamos desde el año 99
                    break;
                case SINO:
                    *banderasino = !*banderasino;
                    break;
                case TIEMPOENCENDIDO:
                    //haycambio = 1;
                    if (tiempoencendido > 0) tiempoencendido--; //
                    else
                        if (tiempoencendido == 0) tiempoencendido = TIEMPOMAXIMOENCENDIDO;
                    break;
                case TIEMPOFALLA:
                    //haycambio = 1;
                    if (tiempofalla > 0) tiempofalla--; //
                    else
                        if (tiempofalla == 0) tiempofalla = TIEMPOMAXIMOFALLA;
                    break;
                case PERIODOENCENDIDO:
                    //haycambio = 1;
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
        // </editor-fold>
        // <editor-fold defaultstate="collapsed" desc="boton onoff">


        //bandera_graba_global = 0;
        if (Pulsacion(2, BOTON_ONOFF, SIN_REPETICION, LOGICA_INVERSA)) {
            bandera_graba_global = 1;
            bandera_orden_on_off_bomba = !bandera_orden_on_off_bomba;
        }
        // </editor-fold>
        // <editor-fold defaultstate="collapsed" desc="boton menu">
        if (Pulsacion(3, BOTON_MENU, SIN_REPETICION, LOGICA_INVERSA)) {

            buzzer_on(3);
            if (menuactual < ULTIMOMENU)
                menuactual++;
            else
                menuactual = 0;
        }
        // </editor-fold>
        // <editor-fold defaultstate="collapsed" desc="boton manaut">
        if (Pulsacion(4, BOTON_MANAUT, SIN_REPETICION, LOGICA_INVERSA)) {
            bandera_orden_on_off_bomba = 0;
            bandera_orden_Alarma_bomba = 0;
            manual_automatico = !manual_automatico;
            buzzer_on(3);

        }
        // </editor-fold>
        T0IF = 0;
        // </editor-fold>
    } else {
        ///interrupciones no contempladas
    }
}