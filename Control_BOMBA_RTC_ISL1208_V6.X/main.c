/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
// Real Time Clock & NVRAM
// Hardware isl1208 of Dallas Maxim
// With interface I2C
#ifndef __PICCPRO__
#define __PICCPRO__
#endif

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif
#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include <stdio.h>
#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "Pulsadores.h"
#include "pwm.h"
#include "adcPic16.h"
#include <string.h>
#include "_isl1208.h"
#include "HardI2C.h"
#include "usart1.h"
#include "LCDGeneric.h"

#define USE_INTERRUPTS 1

///////////////////////////////////////////////////////////////////////////////

void lee_y_transmite_date_and_time(void) {
    isl1208_get_date(&fecha.day, &fecha.month, &fecha.yr, &fecha.dow);
    isl1208_get_time(&horarioactual.hrs, &horarioactual.min, &horarioactual.sec);
}

void main() {
    TMR1IE = 0; //   disable_interrupts(int_timer1);
    RCIE = 0; //    disable_interrupts(int_rda);
    RBIE = 0;
    INTE = 0; //disable_interrupts(int_ext);
    ADCON1 = 0x06; //todos puertos digitales
    T0CS = 0; // TMR0 Clock Source Select bit: internal
    PSA = 0; //Prescaler is assigned to the Timer0 module
    OPTION_REGbits.PS = 0b101; //: Prescaler Rate Select bits 1/64
    //setup_timer_1(T1_INTERNAL | T1_DIV_BY_1);
    T1CON = 1;
    CCP1CONbits.CCP1M = 0;
    //configura_USART();
    setup_i2c(I2C_MASTER);

    TRISB = 0b00111111; //Configuro las entradas de las teclas y la entrada del reloj
    PORTB = 0;
    TRISC0 = 0; //Configuro el pin de activacion de la bomba como salida
    TRISD = 0x00;
    PORTD = 0;
    TRISE = 0b00000000;
    PORTE = 0;

    vInitLCD();
    static char * cadena;
    /*sprintf(cadenaamostrar, cadena_esp);
    sprintf(cadenaamostrar2, cadena_esp);
    vGotoxyLCD(1, 1);
    cadena = cadenaamostrar;
    while (*cadena != '\0')
        vLCD_Putc(*cadena++);
    vGotoxyLCD(1, 2);
    cadena = cadenaamostrar2;
    while (*cadena != '\0')
        vLCD_Putc(*cadena++);
    refrescadisplay = 0;
    __delay_ms(500);
    sprintf(cadenaamostrar, "Ini...");
    vGotoxyLCD(1, 1);
    cadena = cadenaamostrar;
    while (*cadena != '\0')
        vLCD_Putc(*cadena++);
    vGotoxyLCD(1, 2);
    cadena = cadenaamostrar2;
    while (*cadena != '\0')
        vLCD_Putc(*cadena++);
    refrescadisplay = 0;
     */

    set_periodo_buzzer(); //configuro el pwm para una frecuencia de 3khz
    buzzer_on();
    __delay_ms(200);
    buzzer_off();
    __delay_ms(200);
    buzzer_on();
    __delay_ms(200);
    buzzer_off();

    setADCChannel(MIDECORRIENTE);
    __delay_us(20);
    openADC();

    // <editor-fold defaultstate="collapsed" desc="Lectura del estado actual del RTC">

    if (ISL1208_ready()) {
        sprintf(cadenaamostrar, "RTC OK");

    } else {
        sprintf(cadenaamostrar, "RTC ERRO");
    };
    vGotoxyLCD(1, 1);
    cadena = cadenaamostrar;
    while (*cadena != '\0')
        vLCD_Putc(*cadena++);

    __delay_ms(500);
    __delay_ms(500);
    __delay_ms(500);

    fecha.day = 1;
    fecha.month = 1;
    fecha.yr = 15;
    fecha.dow = 4;

    isl1208SR.Valor = 0x00;
    isl1208SR.Valor = ISL1208_Read_status();
    if (isl1208SR.RTCF) {//Si se reseteo el RTC, envio directamente a configurar la hora
        isl1208_init();
        menuactual = MENU_CONFIGURAHORARIO;

    } else {

        lee_y_transmite_date_and_time();

        /*TODO leer el valor de la alarma de encendido en algun lado cuando se cambia el
         valor de horarioenc pero no se guarda
         */
        isl1208_get_time_enc(&horarioenc.hrs, &horarioenc.min, &horarioenc.sec);
        isl1208_get_dow_enc(&fechaenc.dow);

        if (isl1208SR.ALM) {
            //TODO se activo la alarma cuando estaba apagado, procesar

        }
    }
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Lectura de datos guardados en EEPROM">
    periodoencendido = eeprom_read(0);
    if (periodoencendido > TIEMPOMAXIMOPERIODO) periodoencendido = 1;
    tiempoencendido = eeprom_read(1);
    if (tiempoencendido > TIEMPOMAXIMOENCENDIDO) tiempoencendido = 15;
    usa_falla_de_corriente = eeprom_read(2);
    if (usa_falla_de_corriente > 1) usa_falla_de_corriente = 1;
    usa_nivel_bajo = eeprom_read(3);
    if (usa_nivel_bajo > 1)usa_nivel_bajo = 1;
    tiempofalla = eeprom_read(4);
    if (tiempofalla > TIEMPOMAXIMOFALLA)tiempofalla = 5;
    //si se produjo un error en la EEPROM, asigno valores predefinidos
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Habilita Interrupciones">

    interruptADC_on();
    INTEDG = 0;
    INTE = 1; // enable_interrupts(int_ext2);
    TMR0IE = 1;
    ADIF = 0;
    INTF = 0; // borro las banderas de interrupcion
    TMR0IF = 0;
    PEIE = 1;
    ei(); //enable_interrupts(global);
    // </editor-fold>

    while (1) {
        // <editor-fold defaultstate="collapsed" desc="Inicia Procesa los menú">

        ////////////////////////////////////////
        switch (menuactual) {
            case MENU_MUESTRAHORA:
            {
                horario = &horarioactual;
                //Se actualiza lo que se muestra en el display, solamente cuando hay cambios en lo que mostrar

                if (flanco) {
                    lee_y_transmite_date_and_time();
                    sprintf(cadenaamostrar, "%02d:%02d    ", horarioactual.hrs, horarioactual.min);
                    sprintf(cadenaamostrar2, cadena_esp);
                } else {
                    sprintf(cadenaamostrar, "%02d %02d    ", horarioactual.hrs, horarioactual.min);
                    sprintf(cadenaamostrar2, cadena_esp);
                }
                break;
            }
            case MENU_MUESTRAFECHA:
            {

                if (flanco) {
                    lee_y_transmite_date_and_time();
                    sprintf(cadenaamostrar, "%02d/%02d/%02d ", fecha.day, fecha.month, fecha.yr);
                    sprintf(cadenaamostrar2, cadena_esp);
                    strncpy(cadenaamostrar2, days_of_week[dia_de_la_semana(&fecha.day, &fecha.month, &fecha.yr)], 2);
                }


                break;
            }
            case MENU_CONFIGURAHORARIO:
            {

                if (flanco) {
                    sprintf(cadenaamostrar, "SET HORA ");
                    sprintf(cadenaamostrar2, cadena_esp);
                }

                break;
            }
            case SUBMENU_CONFIGURAHORA:
            {
                modificafecha = HORA;
                horario = &horarioactual;
                if (flanco || haycambio) {
                    sprintf(cadenaamostrar, "%02d:%02d    ", horarioactual.hrs, horarioactual.min);
                    sprintf(cadenaamostrar2, cadena_esp);
                    haycambio = 0;
                } else {
                    sprintf(cadenaamostrar, "  :%02d    ", horarioactual.min);
                    sprintf(cadenaamostrar2, cadena_esp);
                }
                bandera_graba_hora = 1;
                break;
            }
            case SUBMENU_CONFIGURAMINUTOS:
            {
                modificafecha = MINUTOS;
                horario = &horarioactual;
                if (flanco || haycambio) {
                    sprintf(cadenaamostrar, "%02d:%02d    ", horarioactual.hrs, horarioactual.min);
                    sprintf(cadenaamostrar2, cadena_esp);
                    haycambio = 0;
                } else {
                    sprintf(cadenaamostrar, "%02d:      ", horarioactual.hrs);
                    sprintf(cadenaamostrar2, cadena_esp);
                }
                bandera_graba_hora = 1;
                break;
            }
            case MENU_CONFIGURAFECHA:
            {

                if (flanco) {
                    sprintf(cadenaamostrar, "SET FECHA");
                    sprintf(cadenaamostrar2, cadena_esp);
                }
                break;
            }
            case SUBMENU_CONFIGURADIA:
            {
                modificafecha = DIA;

                if (flanco || haycambio) {
                    sprintf(cadenaamostrar, "%02d/%02d/%02d ", fecha.day, fecha.month, fecha.yr);
                    sprintf(cadenaamostrar2, days_of_week[dia_de_la_semana(&fecha.day, &fecha.month, &fecha.yr)]);
                    //sprintf(cadenaamostrar2, "             ");
                    haycambio = 0;
                } else {
                    sprintf(cadenaamostrar, "  /%02d/%02d ", fecha.month, fecha.yr);
                    sprintf(cadenaamostrar2, cadena_esp);
                }
                bandera_graba_fecha = 1;
                break;
            }
            case SUBMENU_CONFIGURAMES:
            {
                modificafecha = MES;

                if (flanco || haycambio) {
                    sprintf(cadenaamostrar, "%02d/%02d/%02d ", fecha.day, fecha.month, fecha.yr);
                    sprintf(cadenaamostrar2, days_of_week[dia_de_la_semana(&fecha.day, &fecha.month, &fecha.yr)]);
                    haycambio = 0;
                } else {
                    sprintf(cadenaamostrar, "%02d/  /%02d ", fecha.day, fecha.yr);
                    sprintf(cadenaamostrar2, cadena_esp);
                }
                bandera_graba_fecha = 1;
                break;
            }
            case SUBMENU_CONFIGURAANIO:
            {
                modificafecha = ANIO;

                if (flanco || haycambio) {
                    sprintf(cadenaamostrar, "%02d/%02d/%02d ", fecha.day, fecha.month, fecha.yr);
                    sprintf(cadenaamostrar2, days_of_week[dia_de_la_semana(&fecha.day, &fecha.month, &fecha.yr)]);
                    haycambio = 0;
                } else {
                    sprintf(cadenaamostrar, "%02d/%02d/   ", fecha.day, fecha.month);
                    sprintf(cadenaamostrar2, cadena_esp);
                }
                bandera_graba_fecha = 1;
                break;
            }
            case MENU_CONFIGURAENCENDIDO:
            {

                if (flanco) {
                    sprintf(cadenaamostrar, "SET ENC  ");
                    sprintf(cadenaamostrar2, cadena_esp);
                }

                break;
            }
            case SUBMENU_CONFIGURAHORAENCENDIDO:
            {
                modificafecha = HORA;
                horario = &horarioenc;
                if (flanco || haycambio) {
                    sprintf(cadenaamostrar, "%02d:%02d    ", horarioenc.hrs, horarioenc.min);
                    sprintf(cadenaamostrar2, cadena_esp);
                    haycambio = 0;
                } else {
                    sprintf(cadenaamostrar, "  :%02d    ", horarioenc.min);
                    sprintf(cadenaamostrar2, cadena_esp);
                }
                bandera_graba_hora = 1;
                break;
            }
            case SUBMENU_CONFIGURAPERIODOENCENDIDO:
            {
                modificafecha = PERIODOENCENDIDO;

                if (flanco || haycambio) {
                    sprintf(cadenaamostrar, "c/%u dias", periodoencendido);
                    sprintf(cadenaamostrar2, cadena_esp);
                    haycambio = 0;
                } else {
                    sprintf(cadenaamostrar, "c/  dias");
                    sprintf(cadenaamostrar2, cadena_esp);
                }
                bandera_graba_periodoencendido = 1;
                break;
            }

            case SUBMENU_CONFIGURATIEMPOENCENDIDO:
            {
                modificafecha = TIEMPOENCENDIDO;

                if (flanco || haycambio) {
                    sprintf(cadenaamostrar, "ENCEN:%02d", tiempoencendido);
                    sprintf(cadenaamostrar2, cadena_esp);
                    haycambio = 0;
                } else {
                    sprintf(cadenaamostrar, "ENCEN:  ");
                    sprintf(cadenaamostrar2, cadena_esp);
                }
                bandera_graba_tiempoencendido = 1;
                break;
            }
            case MENU_CONFIGURAFALLACORRIENTE:
            {
                modificafecha = SINO;
                banderasino = &usa_falla_de_corriente;
                if (flanco)
                    if (usa_falla_de_corriente) {
                        sprintf(cadenaamostrar, "FC:   SI");
                        sprintf(cadenaamostrar2, cadena_esp);
                    } else {
                        sprintf(cadenaamostrar, "FC:   NO");
                        sprintf(cadenaamostrar2, cadena_esp);
                    } else {
                    sprintf(cadenaamostrar, "FC:     ");
                    sprintf(cadenaamostrar2, cadena_esp);
                }
                bandera_graba_usa_falla_de_corriente = 1;
                break;
            }
            case MENU_CONFIGURATIEMPOFALLACORRIENTE:
            {
                modificafecha = TIEMPOFALLA;

                if (flanco || haycambio) {
                    sprintf(cadenaamostrar, "TFC:  %02d", tiempofalla);
                    sprintf(cadenaamostrar2, cadena_esp);
                    haycambio = 0;
                } else {
                    sprintf(cadenaamostrar, "TFC:    ");
                    sprintf(cadenaamostrar2, cadena_esp);
                }
                bandera_graba_tiempofalla = 1;
                break;
            }
            case MENU_CONFIGURAINDICACIONDENIVEL:
            {
                modificafecha = SINO;
                banderasino = &usa_nivel_bajo;
                if (flanco)
                    if (usa_nivel_bajo) {
                        sprintf(cadenaamostrar, "NIVEL:SI");
                        sprintf(cadenaamostrar2, cadena_esp);
                    } else {
                        sprintf(cadenaamostrar, "NIVEL:NO");
                        sprintf(cadenaamostrar2, cadena_esp);
                    } else {
                    sprintf(cadenaamostrar, "NIVEL:  ");
                    sprintf(cadenaamostrar2, cadena_esp);
                }
                bandera_graba_usa_nivel_bajo = 1;
                break;
            }
            case MENU_MUESTRAMEDICIONES:
            {
                //convierto el valor decimal a float
                //para una entrada de 3.3V la lectura es 1023
                //para 0 V la lectura es 0
                //para una maxima lectura quiero que en el display se observe 50A
                //Deseo que tambien me muestre 1 decimal, separo la parte entera y la decimal
                //como la resolucion es 1024, cada bit corresponde a 50A/1024=0.048828125A
                //cada 20 muestras tengo aproximdamente 1A

                adcenteroI = (unsigned int) mediciondecorriente;
                adcdecimalI = (unsigned int) ((mediciondecorriente - (unsigned int) mediciondecorriente)*10);
                sprintf(cadenaamostrar, "%2u.%uA   ", (unsigned int) adcenteroI, (unsigned int) adcdecimalI);
                sprintf(cadenaamostrar2, "%3uV    ", (unsigned int) adcenteroV);
                break;
            }
            default:
                break;
        };
        //////////////////////////////////////////////////////////
        //Finaliza Procesa los menú
        // </editor-fold>

        // <editor-fold defaultstate="collapsed" desc="Muestra estado de la bomba">
        //Verifica estados de falla
        /////////////////////////////////////////////////////////
        if (menuactual != MENU_MUESTRAMEDICIONES && menuactual != MENU_MUESTRAFECHA && (menuactual < SUBMENU_CONFIGURADIA || menuactual > SUBMENU_CONFIGURAANIO)) {
            switch (estadobomba) {
                case BOMBAAPAGADA:
                {
                    sprintf(cadenaamostrar2, "     off");
                    break;
                }
                case BOMBAENCENDIDA:
                {
                    sprintf(cadenaamostrar2, "      on");
                    break;
                }
                default:
                    break;
            }
            switch (manual_automatico) {
                case MANUAL:
                {
                    strncpy(cadenaamostrar2, "MAN:", 4);
                    break;
                }
                case AUTOMATICO:
                {
                    strncpy(cadenaamostrar2, "AUT:", 4);
                    break;
                }
                default:
                    break;
            }
        }
        // </editor-fold>

        // <editor-fold defaultstate="collapsed" desc="Verifica estados de falla">


        if ((estadonivel == NIVELNORMAL) && (estadofallacorriente == CORRIENTENORMAL) && (estadofallavoltaje == VOLTAJENORMAL)) {

            // activabomba = ENCIENDEBOMBA;
            //TODO acá debo dar la orden para encender la bomba?
        } else {
            activabomba = APAGABOMBA;
        }
        //////////////////////////////////////////////////////
        //Fin Verifica estados de falla
        // </editor-fold>

        // <editor-fold defaultstate="collapsed" desc="Inicio de procesamiento de medicion de voltaje, corriente">
        //Inicio de procesamiento de medicion de voltaje, corriente
        /////////////////////////////////////////////////////////////
        //convierto el valor decimal a float
        //para una entrada de 3.3V la lectura es 1023
        //para 0 V la lectura es 0
        //para una maxima lectura quiero que en el display se observe 50A
        //Deseo que tambien me muestre 1 decimal, separo la parte entera y la decimal
        //como la resolucion es 1024, cada bit corresponde a 50A/1024=0.048828125A
        //cada 20 muestras tengo aproximdamente 1A
        mediciondevoltaje = (float) medidaV_adc * 440 / 1024;
        adcenteroV = (unsigned int) mediciondevoltaje;
        //Se lee el voltaje de alimentación constantemente
        if (adcenteroV <= VOLTAJEMAXIMO && adcenteroV >= VOLTAJEMINIMO) {
            estadofallavoltaje = VOLTAJENORMAL;
        } else {
            estadofallavoltaje = FALLAVOLTAJE;
        }
        mediciondecorriente = (float) medidaI_adc * 50 / 1024;
        /////////////////////////////////////////////////////////////
        //Fin de procesamiento de medicion de voltaje, corriente
        // </editor-fold>

        // <editor-fold defaultstate="collapsed" desc="Activa o desactiva la Bomba">
        //Activa o desactiva la Bomba
        ///////////////////////////////////////////////////////
        switch (activabomba) {
            case ENCIENDEBOMBA:
            {
                salidabomba = 1;
                estadobomba = 1;
                break;
            }
            case APAGABOMBA:
            {
                salidabomba = 0;
                estadobomba = 0;
                break;
            }
            default:
                break;
        }
        //////////////////////////////////////////////////////
        //Fin Activa o desactiva la Bomba
        // </editor-fold>

        // <editor-fold defaultstate="collapsed" desc="Procesa Alarma">
        //Inicio Procesa Alarma
        ///////////////////////////////////////////////////////
        switch (alarma_encendido) {
            case NOALARMA:
            {
                break;
            }
            case SIALARMA:
            {
                bandera_orden_Alarma_bomba = 1;
                //activabomba = ENCIENDEBOMBA;
                buzzer_on();
                alarma_encendido = NOALARMA;
                isl1208SR.Valor = ISL1208_Read_status();
                isl1208SR.ALM = 0; //reseteo la indicacion de alarma del RTC
                ISL1208_Set_status(&isl1208SR.Valor);
                break;
            }
            default:
                break;
        }
        //////////////////////////////////////////////////////
        //Fin Procesa Alarma
        // </editor-fold>

        // <editor-fold defaultstate="collapsed" desc="Verifica secuencia de arranque">
        /*para que el motor arranque se debe cumplir que:
         * el voltaje sea normal,
         * si está en manual, se haya pulsado el boton on_off
         * si está en automatico se haya activado la alarma por interrupcion
         * el nivel no sea bajo,
         * la corriente sea normal, por lo menos despues de un tiempo despues del arranque
         */
        if (estadofallavoltaje == VOLTAJENORMAL) {
            if (manual_automatico == MANUAL) {
                if (bandera_orden_on_off_bomba) {
                    if (indica_secuencia_arranque == 0) {
                        if (tiempo_secuencia_arranque == 0) {
                            activabomba = ENCIENDEBOMBA; //Enciendo la bomba para empezar a medir la corriente
                            estadofallacorriente = CORRIENTENORMAL;
                            estadonivel = NIVELNORMAL;
                            tiempo_secuencia_arranque = 15; //TODO ajustar el tiempo de secuencia de arranque
                        } else {
                            //TODO Un a vez que se activo la bomba debo ver el estado de la corriente  para ver si no se pasa de los valores normales
                            if (mediciondecorriente <= CORRIENTEMAXIMA) {
                                estadofallacorriente = CORRIENTENORMAL;
                            } else {
                                estadofallacorriente = FALLACORRIENTE;
                            }

                        }
                    } else {
                        //TODO ya finalizo la secuencia de arranque, entonces veo si se estabilizó la corriente
                        if (mediciondecorriente <= CORRIENTENORMALMAXIMA) {
                            estadofallacorriente = CORRIENTENORMAL;
                            cuenta_tiempofalla = 0;
                        } else {
                            //aca verifica si la falla está presente mucho tiempo con tiempofalla
                            if (indica_tiempo_falla == 0) {
                                if (cuenta_tiempofalla == 0) {
                                    cuenta_tiempofalla = tiempofalla;
                                }
                            } else {
                                //Expiro el tiempo de falla con una falla de sobrecorriente, debo apagar la bomba
                                estadofallacorriente = FALLACORRIENTE;
                                indica_tiempo_falla = 0;
                                cuenta_tiempofalla = 0;
                                indica_secuencia_arranque = 0;
                                tiempo_secuencia_arranque = 0;
                                bandera_orden_on_off_bomba = 0; //cambia la bandera de orden de encendido de bomba
                            }


                        }
                        if (mediciondecorriente >= CORRIENTENORMALMINIMA) {
                            estadonivel = NIVELNORMAL;

                        } else {
                            indica_secuencia_arranque = 0;
                            tiempo_secuencia_arranque = 0;
                            bandera_orden_on_off_bomba = 0;
                            estadonivel = NIVELBAJO;
                        }
                    }
                } else {
                    //TODO En caso de haber falla tambien debo resetear estos valores?
                    indica_secuencia_arranque = 0;
                    tiempo_secuencia_arranque = 0;
                    activabomba = APAGABOMBA;
                }
            } else {
                if (bandera_orden_Alarma_bomba) {
                    if (indica_secuencia_arranque == 0) {
                        if (tiempo_secuencia_arranque == 0) {
                            activabomba = ENCIENDEBOMBA; //Enciendo la bomba para empezar a medir la corriente
                            estadofallacorriente = CORRIENTENORMAL;
                            estadonivel = NIVELNORMAL;
                            tiempo_secuencia_arranque = 15; //TODO ajustar el tiempo de secuencia de arranque
                        } else {
                            //TODO Un a vez que se activo la bomba debo ver el estado de la corriente  para ver si no se pasa de los valores normales
                            if (mediciondecorriente <= CORRIENTEMAXIMA) {
                                estadofallacorriente = CORRIENTENORMAL;
                            } else {
                                estadofallacorriente = FALLACORRIENTE;
                            }

                        }
                    } else {
                        //TODO ya finalizo la secuencia de arranque, entonces veo si se estabilizó la corriente
                        if (mediciondecorriente <= CORRIENTENORMALMAXIMA) {
                            estadofallacorriente = CORRIENTENORMAL;
                            cuenta_tiempofalla = 0;
                        } else {
                            //aca verifica si la falla está presente mucho tiempo con tiempofalla
                            if (indica_tiempo_falla == 0) {
                                if (cuenta_tiempofalla == 0) {
                                    cuenta_tiempofalla = tiempofalla;
                                }
                            } else {
                                //Expiro el tiempo de falla con una falla de sobrecorriente, debo apagar la bomba
                                estadofallacorriente = FALLACORRIENTE;
                                indica_tiempo_falla = 0;
                                cuenta_tiempofalla = 0;
                                indica_secuencia_arranque = 0;
                                tiempo_secuencia_arranque = 0;
                                bandera_orden_Alarma_bomba = 0; //cambia la bandera de orden de encendido de bomba
                            }


                        }
                        if (mediciondecorriente >= CORRIENTENORMALMINIMA) {
                            estadonivel = NIVELNORMAL;
                            //TODO acá debo apagar la bomba cuando pase el tiempo de encendido de la bomba
                        } else {
                            indica_secuencia_arranque = 0;
                            tiempo_secuencia_arranque = 0;
                            bandera_orden_Alarma_bomba = 0;
                            estadonivel = NIVELBAJO;
                        }
                    }
                } else {
                    indica_secuencia_arranque = 0;
                    tiempo_secuencia_arranque = 0;
                    activabomba = APAGABOMBA;
                }
            }


        }



        // </editor-fold>

        // <editor-fold defaultstate="collapsed" desc="Actualiza Display">
        //Actualiza Display
        /////////////////////////////////////////////
        if (refrescadisplay) {
            vGotoxyLCD(1, 1);
            cadena = cadenaamostrar;
            while (*cadena != '\0')
                vLCD_Putc(*cadena++);
            vGotoxyLCD(1, 2);
            cadena = cadenaamostrar2;
            while (*cadena != '\0')
                vLCD_Putc(*cadena++);

            refrescadisplay = 0;
        }
        ////////////////////////////////////////////////
        //Fin Actualiza Display
        // </editor-fold>

        // <editor-fold defaultstate="collapsed" desc="Graba datos">
        // <editor-fold defaultstate="collapsed" desc="Graba Nuevos datos en el RTC">
        //Graba Nuevos datos en el RC
        /////////////////////////////////////////////////
        if (bandera_graba_global) {
            if (bandera_graba_hora) {
                if (horario == &horarioactual) {
                    isl1208_set_time((*horario).hrs, (*horario).min, 00);
                    buzzer_on();
                }
                if (horario == &horarioenc) {
                    isl1208_set_time_enc((*horario).hrs, 00, 00);
                    //TODO isl1208_set_dow_enc(&fechaenc.dow);
                    buzzer_on();
                }
            }
            if (bandera_graba_fecha) {
                isl1208_set_date(&fecha.day, &fecha.month, &fecha.yr, &fecha.dow);
                buzzer_on();
            }
        }
        bandera_graba_hora = 0;
        bandera_graba_fecha = 0;
        //////////////////////////////////////////////////////////////////
        //Finaliza Graba Nuevos datos en el RTC
        // </editor-fold>

        // <editor-fold defaultstate="collapsed" desc="Graba Nuevos datos en la EEPROM DEL PIC">
        //DIRECCION 0: periodoencendido(cada cuanto se enciende la bomba en automatico), de 0 a 6 dias,
        //DIRECCION 1: tiempoencendido (cuanto tiempo permanece encendida la bomba en automatico) 0 a 60 minutos
        //DIRECCION 2: usa_falla_de_corriente (dice si se usa falla de corriente o no)
        //DIRECCION 3: usa_nivel_bajo (indica si se usa la deteccion de nivel bajo)
        //DIRECCION 4: tiempofalla (cuanto tiempo puede estar en falla de corriente antes de apagar la bomba por seguridad) 0 a 10 segundos


        if (bandera_graba_global) {
            di();
            if (bandera_graba_periodoencendido) {
                eeprom_write(0, periodoencendido);
                buzzer_on();
            }
            if (bandera_graba_tiempoencendido) {
                eeprom_write(1, tiempoencendido);
                buzzer_on();
            }
            if (bandera_graba_usa_falla_de_corriente) {
                eeprom_write(2, usa_falla_de_corriente);
                buzzer_on();
            }
            if (bandera_graba_usa_nivel_bajo) {
                eeprom_write(3, usa_nivel_bajo);
                buzzer_on();
            }
            if (bandera_graba_tiempofalla) {
                eeprom_write(4, tiempofalla);
                buzzer_on();
            }
            ei();
        }
        bandera_graba_global = 0;
        bandera_graba_periodoencendido = 0;
        bandera_graba_tiempoencendido = 0;
        bandera_graba_usa_falla_de_corriente = 0;
        bandera_graba_usa_nivel_bajo = 0;
        bandera_graba_tiempofalla = 0;
        // </editor-fold>
        // </editor-fold>
        NOP();
        NOP();
    }
}

