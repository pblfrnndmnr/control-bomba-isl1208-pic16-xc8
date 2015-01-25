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
/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/* i.e. uint8_t <variable_name>; */

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

//#fuses HS,MCLR,NOWDT,NOPROTECT,NOPUT,NOBROWNOUT,NOPBADEN,NOLVP,NOCPD,NODEBUG,NOWRT,NOVREGEN
//#use delay(clock=20000000)
//#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)


#define USE_INTERRUPTS 1

#include "_isl1208.h"
#include "HardI2C.h"
#include "usart1.h"
#include "LCDGeneric.h"
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
    //setup_counters(RTCC_INTERNAL,RTCC_DIV_2);
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
    sprintf(cadenaamostrar, cadena_esp);
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


    set_periodo_buzzer(); //configuro el pwm para una frecuencia de 3khz
    buzzer_on();
    __delay_ms(200);
    buzzer_off();
    __delay_ms(200);
    buzzer_on();
    __delay_ms(200);
    buzzer_off();
    //ext_int_edge(2,H_TO_L);
    INTEDG = 0;
    INTE = 1; // enable_interrupts(int_ext2);
    TMR0IE = 1;
    setADCChannel(MIDECORRIENTE);
    __delay_us(20);
    openADC();
    __delay_us(20);
    interruptADC_on();
    __delay_ms(500);



    if (ISL1208_ready()) {
        sprintf(cadenaamostrar, "RTC OK");
        vGotoxyLCD(1, 1);
        cadena = cadenaamostrar;
        while (*cadena != '\0')
            vLCD_Putc(*cadena++);

        __delay_ms(500);
        __delay_ms(500);
        __delay_ms(500);
    } else {
        sprintf(cadenaamostrar, "RTC ERRO");
        vGotoxyLCD(1, 1);
        cadena = cadenaamostrar;
        while (*cadena != '\0')
            vLCD_Putc(*cadena++);

        __delay_ms(500);
        __delay_ms(500);
        __delay_ms(500);

    };
    fecha.day = 1;
    fecha.month = 1;
    fecha.yr = 15;
    fecha.dow = 4;
    isl1208SR.Valor = 0x00;
    isl1208SR.Valor = ISL1208_Read_status();
    if (isl1208SR.RTCF) {//Si se reseteo el RTC, envio directamente a configurar la hora
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

    ADIF = 0;
    INTF = 0; // borro las banderas de interrupcion
    TMR0IF = 0;
    PEIE=1;
    ei(); //enable_interrupts(global);
    // Inicializa isl1208
    //TODO   isl1208_init(isl1208_OUT_ENABLED | isl1208_OUT_1_HZ);

    while (1) {//
        // di(); //
        //Inicia Procesa los menú
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
                    sprintf(cadenaamostrar2, days_of_week[dia_de_la_semana(&fecha.day, &fecha.month, &fecha.yr)]);
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

                break;
            }
            case SUBMENU_CONFIGURAMINUTOSENCENDIDO:
            {
                modificafecha = MINUTOS;
                horario = &horarioenc;
                if (flanco || haycambio) {
                    sprintf(cadenaamostrar, "%02d:%02d    ", horarioenc.hrs, horarioenc.min);
                    sprintf(cadenaamostrar2, cadena_esp);
                    haycambio = 0;
                } else {
                    sprintf(cadenaamostrar, "%02d:      ", horarioenc.hrs);
                    sprintf(cadenaamostrar2, cadena_esp);
                }
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
                break;
            }
            case MENU_CONFIGURAFALLACORRIENTE:
            {
                modificafecha = SINO;

                if (flanco)
                    if (banderasino) {
                        sprintf(cadenaamostrar, "NORMAL A ");
                        sprintf(cadenaamostrar2, cadena_esp);
                    } else {
                        sprintf(cadenaamostrar, "NORMAL C ");
                        sprintf(cadenaamostrar2, cadena_esp);
                    } else {
                    sprintf(cadenaamostrar, "NORMAL   ");
                    sprintf(cadenaamostrar2, cadena_esp);
                }
                break;
            }
            case MENU_CONFIGURATIEMPOFALLACORRIENTE:
            {
                modificafecha = TIEMPOFALLA;

                if (flanco || haycambio) {
                    sprintf(cadenaamostrar, "FALLA:%02d", tiempofalla);
                    sprintf(cadenaamostrar2, cadena_esp);
                    haycambio = 0;
                } else {
                    sprintf(cadenaamostrar, "FALLA:  ");
                    sprintf(cadenaamostrar2, cadena_esp);
                }
                break;
            }
            case MENU_CONFIGURAINDICACIONDENIVEL:
            {
                modificafecha = SINO;


                if (flanco)
                    if (banderasino) {
                        sprintf(cadenaamostrar, "NORMAL A ");
                        sprintf(cadenaamostrar2, cadena_esp);
                    } else {
                        sprintf(cadenaamostrar, "NORMAL C ");
                        sprintf(cadenaamostrar2, cadena_esp);
                    } else {
                    sprintf(cadenaamostrar, "NORMAL   ");
                    sprintf(cadenaamostrar2, cadena_esp);
                }

                break;
            }
            case MENU_MUESTRAMEDICIONES:
            {
                unsigned int adcenteroI;
                unsigned int adcdecimalI;
                unsigned int adcenteroV;
                mediciondecorriente = (float) medidaI_adc * 50 / 1024;
                //convierto el valor decimal a float
                //para una entrada de 3.3V la lectura es 1023
                //para 0 V la lectura es 0
                //para una maxima lectura quiero que en el display se observe 50A
                //Deseo que tambien me muestre 1 decimal, separo la parte entera y la decimal
                //como la resolucion es 1024, cada bit corresponde a 50A/1024=0.048828125A
                //cada 20 muestras tengo aproximdamente 1A
                float mediciondevoltaje = (float) medidaV_adc * 440 / 1024;
                adcenteroI = (unsigned int) mediciondecorriente;
                adcdecimalI = (unsigned int) ((mediciondecorriente - (unsigned int) mediciondecorriente)*10);
                adcenteroV = (unsigned int) mediciondevoltaje;
                sprintf(cadenaamostrar, "%2u.%uA   ", (unsigned int) adcenteroI, (unsigned int) adcdecimalI);
                sprintf(cadenaamostrar2, "%3uV    ", (unsigned int) adcenteroV);
                break;
            }
            default:
                break;
        };
        //////////////////////////////////////////////////////////
        //Finaliza Procesa los menú

        //Verifica estados de falla
        /////////////////////////////////////////////////////////
        if(menuactual!=MENU_MUESTRAMEDICIONES && menuactual!= MENU_MUESTRAFECHA&& (menuactual<SUBMENU_CONFIGURADIA || menuactual>SUBMENU_CONFIGURAANIO))
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

        if (/*(estadonivel == NIVELNORMAL)&&*/(estadofallacorriente == CORRIENTENORMAL) && (estadofallavoltaje == VOLTAJENORMAL)) {
            if (estadonivel == NIVELNORMAL)
                activabomba = ENCIENDEBOMBA;
            //TODO acá debo dar la orden para encender la bomba?
        } else {
            activabomba = APAGABOMBA;
        }

        switch (estadonivel) {
            case NIVELNORMAL:
            {

                break;
            }
            case NIVELBAJO:
            {
                activabomba = 0;
                break;
            }
            default:
                break;
        }

        switch (estadofallacorriente) {
            case CORRIENTENORMAL:
            {
                break;
            }
            case FALLACORRIENTE:
            {
                activabomba = 0;
                break;
            }
            default:
                break;
        }
        switch (estadofallavoltaje) {
            case VOLTAJENORMAL:
            {
                break;
            }
            case FALLAVOLTAJE:
            {
                activabomba = 0;
                break;
            }
            default:
                break;
        }
        //////////////////////////////////////////////////////
        //Fin Verifica estados de falla

        //Inicio de procesamiento de medicion de voltaje, corriente
        /////////////////////////////////////////////////////////////
        //  unsigned int adcenteroI;
        //unsigned int adcdecimalI;
        unsigned int adcenteroV;

        //convierto el valor decimal a float
        //para una entrada de 3.3V la lectura es 1023
        //para 0 V la lectura es 0
        //para una maxima lectura quiero que en el display se observe 50A
        //Deseo que tambien me muestre 1 decimal, separo la parte entera y la decimal
        //como la resolucion es 1024, cada bit corresponde a 50A/1024=0.048828125A
        //cada 20 muestras tengo aproximdamente 1A
        float mediciondevoltaje = (float) medidaV_adc * 440 / 1024;
        adcenteroV = (unsigned int) mediciondevoltaje;
        //Se lee el voltaje de alimentación constantemente
        if (adcenteroV <= VOLTAJEMAXIMO && adcenteroV >= VOLTAJEMINIMO) {
            estadofallavoltaje = VOLTAJENORMAL;
        } else {
            estadofallavoltaje = FALLAVOLTAJE;
        }

        //Se lee la corriente consumida solamente cuando la bomba está activada
        switch (activabomba) {
            case APAGABOMBA:
            {
                break;
            }
            case ENCIENDEBOMBA:
            {
                mediciondecorriente = (float) medidaI_adc * 50 / 1024;
                //adcenteroI = (unsigned int) mediciondecorriente;
                //adcdecimalI = (unsigned int) ((mediciondecorriente - (unsigned int) mediciondecorriente)*10);
                if (mediciondecorriente <= CORRIENTEMAXIMA && mediciondecorriente >= CORRIENTEMINIMA) {
                    estadofallacorriente = CORRIENTENORMAL;
                    estadonivel = NIVELNORMAL;

                } else {
                    estadofallacorriente = FALLACORRIENTE;
                    estadonivel = NIVELBAJO;

                }
                break;
            }
            default:
                break;
        }

        // sprintf(cadenaamostrar, "%2u.%uA   ", (unsigned int) adcenteroI, (unsigned int) adcdecimalI);
        // sprintf(cadenaamostrar2, "%3uV    ", (unsigned int) adcenteroV);
        /////////////////////////////////////////////////////////////
        //Fin de procesamiento de medicion de voltaje, corriente


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

        //Graba Nuevos datos en el RC
        /////////////////////////////////////////////////
        if (bandera_startglobal) {
            bandera_grabafechay_hora = 1;

        }
        if (!bandera_startglobal && bandera_grabafechay_hora) {
            if (horario == &horarioactual) {
                isl1208_set_time((*horario).hrs, (*horario).min, 00);
                isl1208_set_date(&fecha.day, &fecha.month, &fecha.yr, &fecha.dow);
            }
            if (horario == &horarioenc) {
                isl1208_set_time_enc((*horario).hrs, (*horario).min, 00);
                //isl1208_set_date(&fecha.day, &fecha.month, &fecha.yr, &fecha.dow);
                isl1208_set_dow_enc(&fechaenc.dow);
            }
            bandera_grabafechay_hora = 0;
        }
        //////////////////////////////////////////////////////////////////
        //Finaliza Graba Nuevos datos en el RTC
        NOP();
        NOP();

    }
}

