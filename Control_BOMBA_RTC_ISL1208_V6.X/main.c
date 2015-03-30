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

void Obtiene_tiempo_restante_de_encendido(void) {

    if (fechaenc.dow >= fecha.dow) {
        tiemporestantedias = fechaenc.dow - fecha.dow;
    }
    if (fechaenc.dow < fecha.dow) {
        tiemporestantedias = fechaenc.dow + (7 - fecha.dow);
    }


    if (horarioactual.hrs <= horarioenc.hrs) {
        //si la hora de apagado es mayor que el horario actual significa que estamos en el mismo día
        //solo tengo que restar las horas
        tiemporestantehora.hrs = horarioenc.hrs - horarioactual.hrs;
    } else {
        //sino lo que hago es calcular el tiempo desde el horario actual hasta las 20hs de la noche y a eso le sumoola hora de apagado
        tiemporestantehora.hrs = horarioenc.hrs + (24 - horarioactual.hrs);
        /* if (tiemporestantedias > 0) {
             tiemporestantedias = tiemporestantedias - 1;
         } else {
             tiemporestantedias = 0;
         }*/
    }

    if (horarioactual.min <= horarioenc.min) {
        //si los minutos de apagado ee mayor que el minuto actual significa que estamos en la misma hora
        //solo tengo que restar las horas
        tiemporestantehora.min = horarioenc.min - horarioactual.min;
    } else {
        //sino lo que hago es calcular el tiempo desde el horario actual hasta los 60 minutos y a eso le sumo los minutos de enc
        tiemporestantehora.min = horarioenc.min + (60 - horarioactual.min);
        //le resto 1 a las horas
        if (tiemporestantehora.hrs > 0) {
            tiemporestantehora.hrs = tiemporestantehora.hrs - 1;
        } else {
            tiemporestantehora.hrs = 23;
            if (tiemporestantedias > 0) {
                tiemporestantedias = tiemporestantedias - 1;
            } else {
                tiemporestantedias = 0;
            }
        }
    }

    if (periodoencendido == 0) {
        tiemporestantedias = 0;
    }

}

void main() {
    TMR1IE = 0; //   disable_interrupts(int_timer1);
    RCIE = 0; //    disable_interrupts(int_rda);
    RBIE = 0;
    INTE = 0; //disable_interrupts(int_ext);
    T0CS = 0; // TMR0 Clock Source Select bit: internal
    PSA = 0; //Prescaler is assigned to the Timer0 module
    OPTION_REGbits.PS = 0b101; //: Prescaler Rate Select bits 1/64
    //setup_timer_1(T1_INTERNAL | T1_DIV_BY_1);
    T1CON = 0; //deshabilito el Timer1
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
    set_periodo_buzzer(); //configuro el pwm para una frecuencia de 2.44khz
    buzzer_on(3);
    __delay_ms(200);
    buzzer_off();
    __delay_ms(200);
    buzzer_on(3);
    __delay_ms(200);
    buzzer_off();

    setADCChannel(MIDECORRIENTE);
    __delay_us(20);
    openADC();

    
    // <editor-fold defaultstate="collapsed" desc="Lectura de datos guardados en EEPROM">
    periodoencendido = eeprom_read(0);
    if (periodoencendido > TIEMPOMAXIMOPERIODO) periodoencendido = 0;
    tiempoencendido = eeprom_read(1);
    if (tiempoencendido > TIEMPOMAXIMOENCENDIDO) tiempoencendido = 15;
    usa_falla_de_corriente = eeprom_read(2);
    if (usa_falla_de_corriente > 1) usa_falla_de_corriente = 1;
    tiempofalla = eeprom_read(3);
    if (tiempofalla > TIEMPOMAXIMOFALLA)tiempofalla = 5;
    manual_automatico = eeprom_read(4);
    if (manual_automatico != MANUAL) {
        manual_automatico = AUTOMATICO;
    }
    //si se produjo un error en la EEPROM, asigno valores predefinidos
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Lectura del estado actual del RTC">

    if (ISL1208_ready()) {
        sprintf(cadenaamostrar, "RTC OK");

    } else {
        sprintf(cadenaamostrar, "RTC ERRO");
    };
    tiempoapagadolcd = 15; //enciende el backlight del lcd por 15 segundos
    vBackLightLCD_On();

    vGotoxyLCD(1, 1);
    cadena = cadenaamostrar;
    while (*cadena != '\0')
        vLCD_Putc(*cadena++);

    __delay_ms(500);

    fecha.day = 1;
    fecha.month = 1;
    fecha.yr = 15;
    fecha.dow = 4;

    isl1208SR.Valor = 0x00;
    isl1208SR.Valor = ISL1208_Read_status();
    if (isl1208SR.RTCF) {//Si se reseteo el RTC, envio directamente a configurar la hora
        isl1208_init();
        menuactual = SUBMENU_CONFIGURAHORA;
        reseteafallas = FALLAPRESENTE;
        ultimafalla = HUBOFALLARTC;
    } else {

        lee_y_transmite_date_and_time();
        isl1208_get_time_enc(&horarioenc.hrs, &horarioenc.min, &horarioenc.sec);
        isl1208_get_dow_enc(&fechaenc.dow);

        if (isl1208SR.ALM) {
            //Si se activo la alarma cuando estaba apagado el sistema se borra esa bandera
            isl1208SR.ALM = 0; //reseteo la indicacion de alarma del RTC
            ISL1208_Set_status(&isl1208SR.Valor);
            //TODO (implementado no probado )ver si transcurrió el periodoencendido, para activar la bomba cuando corresponda
            //Si se activo la alarma, lo que hago es encender la bomba a la hora grabada,en el mismo dia si la hora actual es menor o al día siguiente si la hora actual es mayor, por única vez
            //desactivando la alarma por dow
            isl1208_set_dow_enc(0x00, 0);
            //
        }
    }
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
            case MENU_INICIAL:
            {
                if (reseteafallas == FALLARESETEADA) {
                    ultimafalla = NOHUBOFALLA;
                    if(manual_automatico!=MANUAL)
                    sprintf(cadenaamostrar, "%1dD%02dH%02dM", tiemporestantedias, tiemporestantehora.hrs, tiemporestantehora.min);
                    else
                    sprintf(cadenaamostrar, "        ");
                    // sprintf(cadenaamostrar, cadena_esp);
                } else {
                    if (ultimafalla == HUBOFALLAVOLTAJE) {
                        sprintf(cadenaamostrar, "Falla: V");
                    } else if (ultimafalla == HUBOFALLACORRIENTE) {
                        sprintf(cadenaamostrar, "Falla: C");
                    } else if (ultimafalla == HUBOFALLANIVEL) {
                        sprintf(cadenaamostrar, "Falla: N");
                    } else if (ultimafalla == HUBOFALLARTC) {
                        sprintf(cadenaamostrar, "Falla: R");
                    }
                }

                break;
            }
            case MENU_MUESTRAHORA_FECHA:
            {
                horario = &horarioactual;
                //Se actualiza lo que se muestra en el display, solamente cuando hay cambios en lo que mostrar

                if (flanco) {
                    sprintf(cadenaamostrar, "%02d:%02d:%02d", horarioactual.hrs, horarioactual.min, horarioactual.sec);
                    sprintf(cadenaamostrar2, "%02d/%02d/%02d", fecha.day, fecha.month, fecha.yr);
                } else {
                    sprintf(cadenaamostrar, "%02d %02d %02d", horarioactual.hrs, horarioactual.min, horarioactual.sec);
                    //sprintf(cadenaamostrar2, "%02d/%02d/%02d", fecha.day, fecha.month, fecha.yr);
                    sprintf(cadenaamostrar2, cadena_esp);
                    cadenaamostrar2[0] = days_of_week[fecha.dow][0];
                    cadenaamostrar2[1] = days_of_week[fecha.dow][1];
                }
                break;
            }

            case SUBMENU_CONFIGURAHORA:
            {
                modificafecha = HORA;
                horario = &horarioactual;
                if (flanco || haycambio) {
                    sprintf(cadenaamostrar, "SET HORA");
                    sprintf(cadenaamostrar2, "%02d:%02d   ", horarioactual.hrs, horarioactual.min);
                } else {
                    sprintf(cadenaamostrar, "SET HORA");
                    sprintf(cadenaamostrar2, "  :%02d   ", horarioactual.min);
                }
                bandera_graba_hora = 1;
                break;
            }
            case SUBMENU_CONFIGURAMINUTOS:
            {
                modificafecha = MINUTOS;
                horario = &horarioactual;
                if (flanco || haycambio) {
                    sprintf(cadenaamostrar, "SET HORA");
                    sprintf(cadenaamostrar2, "%02d:%02d   ", horarioactual.hrs, horarioactual.min);
                } else {
                    sprintf(cadenaamostrar, "SET HORA");
                    sprintf(cadenaamostrar2, "%02d:     ", horarioactual.hrs);
                }
                bandera_graba_hora = 1;
                break;
            }
            case SUBMENU_CONFIGURADIA:
            {
                modificafecha = DIA;

                if (flanco || haycambio) {
                    sprintf(cadenaamostrar, "SET FECH");
                    sprintf(cadenaamostrar2, "%02d/%02d/%02d", fecha.day, fecha.month, fecha.yr);
                } else {
                    sprintf(cadenaamostrar, "SET FECH");
                    sprintf(cadenaamostrar2, "  /%02d/%02d", fecha.month, fecha.yr);
                }
                bandera_graba_fecha = 1;
                break;
            }
            case SUBMENU_CONFIGURAMES:
            {
                modificafecha = MES;

                if (flanco || haycambio) {
                    sprintf(cadenaamostrar, "SET FECH");
                    sprintf(cadenaamostrar2, "%02d/%02d/%02d", fecha.day, fecha.month, fecha.yr);
                } else {
                    sprintf(cadenaamostrar, "SET FECH");
                    sprintf(cadenaamostrar2, "%02d/  /%02d", fecha.day, fecha.yr);
                }
                bandera_graba_fecha = 1;
                break;
            }
            case SUBMENU_CONFIGURAANIO:
            {
                modificafecha = ANIO;

                if (flanco || haycambio) {
                    sprintf(cadenaamostrar, "SET FECH");
                    sprintf(cadenaamostrar2, "%02d/%02d/%02d", fecha.day, fecha.month, fecha.yr);
                } else {
                    sprintf(cadenaamostrar, "SET FECH");
                    sprintf(cadenaamostrar2, "%02d/%02d/  ", fecha.day, fecha.month);
                }
                bandera_graba_fecha = 1;
                break;
            }
            case SUBMENU_CONFIGURAHORAENCENDIDO:
            {
                modificafecha = HORA;
                horario = &horarioenc;
                if (flanco || haycambio) {
                    sprintf(cadenaamostrar, "SET ENC ");
                    sprintf(cadenaamostrar2, "%02d:%02d   ", horarioenc.hrs, horarioenc.min);
                } else {
                    sprintf(cadenaamostrar, "SET ENC ");
                    sprintf(cadenaamostrar2, "  :%02d   ", horarioenc.min);
                }
                bandera_graba_hora = 1;
                break;
            }
            case SUBMENU_CONFIGURAMINUTOSENCENDIDO:
            {
                modificafecha = MINUTOS;
                horario = &horarioenc;
                if (flanco || haycambio) {
                    sprintf(cadenaamostrar, "SET ENC ");
                    sprintf(cadenaamostrar2, "%02d:%02d   ", horarioenc.hrs, horarioenc.min);
                } else {
                    sprintf(cadenaamostrar, "SET ENC ");
                    sprintf(cadenaamostrar2, "%02d:     ", horarioenc.hrs);
                }
                bandera_graba_hora = 1;
                break;
            }
            case SUBMENU_CONFIGURAPERIODOENCENDIDO:
            {
                modificafecha = PERIODOENCENDIDO;

                if (flanco || haycambio) {
                    sprintf(cadenaamostrar, "ENC: %02d'", tiempoencendido);
                    sprintf(cadenaamostrar2, "c/%u dias", periodoencendido + 1);
                } else {
                    sprintf(cadenaamostrar, "ENC: %02d'", tiempoencendido);
                    sprintf(cadenaamostrar2, "c/  dias");
                }
                bandera_graba_periodoytiempoencendido = 1;
                break;
            }

            case SUBMENU_CONFIGURATIEMPOENCENDIDO:
            {
                modificafecha = TIEMPOENCENDIDO;

                if (flanco || haycambio) {
                    sprintf(cadenaamostrar, "ENC: %02d'", tiempoencendido);
                    sprintf(cadenaamostrar2, "c/%u dias", periodoencendido + 1);
                } else {
                    sprintf(cadenaamostrar, "ENC:   '");
                    sprintf(cadenaamostrar2, "c/%u dias", periodoencendido + 1);
                }
                bandera_graba_periodoytiempoencendido = 1;
                break;
            }
            case MENU_CONFIGURAFALLACORRIENTE:
            {
                modificafecha = SINO;
                banderasino = &usa_falla_de_corriente;
                if (flanco || haycambio) {
                    if (usa_falla_de_corriente) {
                        sprintf(cadenaamostrar, "FC:   SI");
                        sprintf(cadenaamostrar2, cadena_esp);
                    } else {
                        sprintf(cadenaamostrar, "FC:   NO");
                        sprintf(cadenaamostrar2, cadena_esp);
                    }
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
                    sprintf(cadenaamostrar, "TFC: %02d'", tiempofalla);
                    sprintf(cadenaamostrar2, cadena_esp);
                } else {
                    sprintf(cadenaamostrar, "TFC:   '");
                    sprintf(cadenaamostrar2, cadena_esp);
                }
                bandera_graba_tiempofalla = 1;
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

                adcenteroI = (signed int) mediciondecorriente;
                adcdecimalI = (signed int) ((mediciondecorriente - (signed int) mediciondecorriente)*10);
                sprintf(cadenaamostrar, "%2i.%iA   ", (signed int) adcenteroI, (signed int) adcdecimalI);
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
        /////////////////////////////////////////////////////////
        if (menuactual == MENU_INICIAL) {
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

        if (!usa_falla_de_corriente) {
            estadofallacorriente = CORRIENTENORMAL;
            //TODO Agregado el uso o no de falla de corriente (probar si funciona)
        } else {
            if (estadofallacorriente == FALLACORRIENTE) {
                reseteafallas = FALLAPRESENTE;
                ultimafalla = HUBOFALLACORRIENTE;
                if (estadobomba == BOMBAAPAGADA) {
                    estadofallacorriente = CORRIENTENORMAL;
                }
            }
        }
        if (estadonivel == NIVELBAJO) {
            reseteafallas = FALLAPRESENTE;
            ultimafalla = HUBOFALLANIVEL;
            if (estadobomba == BOMBAAPAGADA) {
                estadonivel = NIVELNORMAL;
            }
        }
        if (estadofallavoltaje == FALLAVOLTAJE) {
            reseteafallas = FALLAPRESENTE;
            ultimafalla = HUBOFALLAVOLTAJE;
        }
        if ((estadonivel == NIVELNORMAL) && (estadofallacorriente == CORRIENTENORMAL) && (estadofallavoltaje == VOLTAJENORMAL)) {


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
        mediciondecorriente = (float) medidaI_adc * 25 / 1024 - OFFSET_I;
        //La ganancia de corriente se ajusta en 25,ya que el TI es de 50A:2A
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
                // vBackLightLCD_On();
                break;
            }
            case APAGABOMBA:
            {
                salidabomba = 0;
                estadobomba = 0;
                // vBackLightLCD_Off();
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
                if (manual_automatico == AUTOMATICO) {
                    bandera_orden_Alarma_bomba = 1;
                    buzzer_on(3);
                } else {
                    bandera_orden_Alarma_bomba = 0;
                }
                alarma_encendido = NOALARMA;
                isl1208SR.Valor = ISL1208_Read_status();
                isl1208SR.ALM = 0; //reseteo la indicacion de alarma del RTC

                ISL1208_Set_status(&isl1208SR.Valor);
                //Leo el valor de la alarma
                isl1208_get_time_enc(&horarioenc.hrs, &horarioenc.min, &horarioenc.sec);
                isl1208_get_dow_enc(&fechaenc.dow);
                /*TODO (implementado no probado )aca debo sumarle a fecha.dow el valor de periodoencendido
                 * para que respete cada cuantos dias se enciende la bomba
                 por ejemplo si la alarma se activo el lunes (1) y periodoencendido+1=3+1 es cada cuatro dias 4
                 debo hacer 1+4=5, 5%7=5 se deberia encender el viernes (5)
                 otro ejemplo si se encendio el jueves (4) y es cada 4 dias, 4+4=8, entonces 8%7=1 (lunes)
                 periodoencendido va de 0 a 6
                 0 corresponde a dia por dia (se debe desactivar la alarma por dow)
                 1 corresponde a cada dos dias
                 2 cada 3 dias
                 ...
                 6 cada 7 dias
                 */
                fechaenc.dow = (fecha.dow + periodoencendido + 1) % 7;
                isl1208_set_dow_enc(&fechaenc.dow, periodoencendido);
                horarioapagado.Valor = horarioenc.Valor;
                if (horarioapagado.min + tiempoencendido >= 60) {
                    horarioapagado.min = horarioapagado.min + tiempoencendido - 60;
                    if (horarioapagado.hrs == 23) {
                        horarioapagado.hrs = 0;
                    } else {
                        horarioapagado.hrs++;
                    }
                } else {
                    horarioapagado.min = horarioapagado.min + tiempoencendido;
                }
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
                            tiempo_secuencia_arranque = TIEMPOSECUENCIAARRANQUE;
                        } else {
                            // Una vez que se activo la bomba debo ver el estado de la corriente  para ver si no se pasa de los valores normales
                            if (mediciondecorriente <= CORRIENTEMAXIMA) {
                                estadofallacorriente = CORRIENTENORMAL;
                            } else {
                                estadofallacorriente = FALLACORRIENTE;
                            }

                        }
                    } else {
                        //ya finalizo la secuencia de arranque, entonces veo si se estabilizó la corriente
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
                            tiempo_secuencia_arranque = TIEMPOSECUENCIAARRANQUE;
                        } else {
                            //Una vez que se activo la bomba debo ver el estado de la corriente  para ver si no se pasa de los valores normales
                            if (mediciondecorriente <= CORRIENTEMAXIMA) {
                                estadofallacorriente = CORRIENTENORMAL;
                            } else {
                                estadofallacorriente = FALLACORRIENTE;
                            }

                        }
                    } else {
                        // ya finalizo la secuencia de arranque, entonces veo si se estabilizó la corriente
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
                            //Acá debo apagar la bomba cuando pase el tiempo de encendido de la bomba
                            if (horarioapagado.hrs == horarioactual.hrs && horarioapagado.min == horarioactual.min) {
                                indica_secuencia_arranque = 0;
                                tiempo_secuencia_arranque = 0;
                                bandera_orden_Alarma_bomba = 0;
                                activabomba = APAGABOMBA;
                            }
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
            haycambio = 0;
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
                    buzzer_on(10);
                }
                if (horario == &horarioenc) {
                    isl1208_set_time_enc((*horario).hrs, (*horario).min, 00);
                    //cuando grabo los datos quiero que toque el dia siguiente o en el mismo dia a la hora especificada
                    //por lo tanto desactivo la alarma por dow
                    //fechaenc.dow = (fecha.dow + periodoencendido + 1) % 7;
                    isl1208_set_dow_enc(0x00, 0);
                    buzzer_on(10);
                }
            }
            if (bandera_graba_fecha) {
                fecha.dow = dia_de_la_semana(&fecha.day, &fecha.month, &fecha.yr);
                isl1208_set_date(&fecha.day, &fecha.month, &fecha.yr, &fecha.dow);
                buzzer_on(10);
            }
        }
        //Si no hay datos para grabar en hora y fecha actualizo la hora y la fecha del RTC
        if (actualizo_datos_rtc && bandera_graba_hora == 0 && bandera_graba_fecha == 0) {
            lee_y_transmite_date_and_time();
            isl1208_get_time_enc(&horarioenc.hrs, &horarioenc.min, &horarioenc.sec);
            isl1208_get_dow_enc(&fechaenc.dow);
            Obtiene_tiempo_restante_de_encendido();
            actualizo_datos_rtc = 0;
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
        //DIRECCION 3: tiempofalla (cuanto tiempo puede estar en falla de corriente antes de apagar la bomba por seguridad) 0 a 10 segundos

        if (bandera_graba_global) {
            di();
            if (bandera_graba_periodoytiempoencendido) {
                isl1208_set_dow_enc(0x00, 0); //comienzo a encender la bomba el día siguiente
                eeprom_write(0, periodoencendido);
                eeprom_write(1, tiempoencendido);
                buzzer_on(10);
            }
            if (bandera_graba_usa_falla_de_corriente) {
                eeprom_write(2, usa_falla_de_corriente);
                buzzer_on(10);
            }
            if (bandera_graba_tiempofalla) {
                eeprom_write(3, tiempofalla);
                buzzer_on(10);
            }
            if (bandera_graba_manual_automatico) {
                eeprom_write(4, manual_automatico);
                buzzer_on(10);
            }
            ei();
        }
        bandera_graba_global = 0;
        bandera_graba_periodoytiempoencendido = 0;
        bandera_graba_usa_falla_de_corriente = 0;
        bandera_graba_tiempofalla = 0;
        bandera_graba_manual_automatico = 0;
        // </editor-fold>
        // </editor-fold>
        NOP();
        NOP();
    }
}

