//Código original bajado de: http://picmania.garcia-cuervo.net/proyectos_aux_rtc.php
//Gracias RedRaven
////////////////////////////////////////////////////////////////////////////////////////
///                               isl1208.C                                          ///
///                     Driver for Real Time Clock                                   ///
///                     modified by Pablo Fernando Manieri - Enero 2015              ///
///                                                                                  ///
/// bool ISL1208_ready(void) - Testea si el RTC está presente                        ///
/// void isl1208_init(val)                                                           ///
///                  - Enable oscillator without clearing the seconds register       ///
///                    used when PIC loses power and isl1208 run from 3V BAT         ///
///                  - Config Control Register with next parameters:                 ///
///                     isl1208_ALL_DISABLED          All disabled                   ///
///                     isl1208_OUT_ON_DISABLED_HIHG  Out to Hight on Disable Out    ///
///                     isl1208_OUT_ENABLED           Out Enabled                    ///
///                     isl1208_OUT_1_HZ              Freq. Out to 1 Hz              ///
///                     isl1208_OUT_4_KHZ             Freq. Out to 4.096 Khz         ///
///                     isl1208_OUT_8_KHZ             Freq. Out to 8.192 Khz         ///
///                     isl1208_OUT_32_KHZ            Freq. Out to 32.768 Khz        ///
///                                                                                  ///
///                     Example init:                                                ///
///                     isl1208_init(isl1208_ALL_DISABLED);                          ///
///                     isl1208_init(isl1208_OUT_ENABLED | isl1208_OUT_1_HZ);        ///
///                                                                                  ///
/// void isl1208_set_date_time(day,mth,year,dow,hour,min,sec) - Set the date/time    ///
///                                                                                  ///
/// void isl1208_get_date(day,mth,year,dow) - Get the date                           ///
///                                                                                  ///
/// void isl1208_get_time(hr,min,sec) - Get the time                                 ///
///                                                                                  ///
/// char isl1208_read_nvram_unsigned char(char addr) - Read unsigned char in address ///
///                                                                                  ///
/// void isl1208_write_nvram_unsigned char(char addr, char value) - Write unsigned   ///
///                                                                  char in address ///
///                                                                                  ///
/// void isl1208_get_day_of_week(char* ptr) - Get string Day Of Week                 ///
///                                                                                  ///
///                                                                                  ///
////////////////////////////////////////////////////////////////////////////////////////

#include <xc.h>
#include <stdio.h>
#include <stdbool.h>       /* For true/false definition */
#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "HardI2C.h"
#include "_isl1208.h"
#include "user.h"

bool ISL1208_ready(void) {
    bool ack;

    start_i2c(); // If the write command is acknowledged,
    ack = write_i2c(isl1208_Write); // then the device is ready.
    stop_i2c();
    return !ack; // Invert ACK returned so TRUE = ACK OK
}

void ISL1208_Set_status(unsigned char *SR) {

    start_i2c(); // If the write command is acknowledged,
    write_i2c(isl1208_Write); // then the device is ready.
    write_i2c(0x07);
    write_i2c(*SR);
    stop_i2c();

}


unsigned char ISL1208_Read_status(void) {
    char SR;
    start_i2c(); // If the write command is acknowledged,
    write_i2c(isl1208_Write); // then the device is ready.
    write_i2c(0x07);
    rstart_i2c(); 
    write_i2c(isl1208_Read);
    SR = read_i2c(NOACK);
    stop_i2c();
    return SR;
}

unsigned char ISL1208_Read_Int(void) {
    char Int;
    start_i2c(); // If the write command is acknowledged,
    write_i2c(isl1208_Write); // then the device is ready.
    write_i2c(0x08);
    rstart_i2c();
    write_i2c(isl1208_Read);
    Int = read_i2c(NOACK);
    stop_i2c();
    return Int;
}

void ISL1208_Set_Int(unsigned char *Int) {

    start_i2c(); // If the write command is acknowledged,
    write_i2c(isl1208_Write); // then the device is ready.
    write_i2c(0x08);
    write_i2c(*Int);
    stop_i2c();
   
}



void isl1208_init() {

isl1208INT.ALME=1;//habilita la alarma
isl1208INT.FO=0;//deshabilita la salida de frecuencia
isl1208INT.FOBATB=1;//deshabilita la salida de fout/irq en modo backup
isl1208INT.IM=1;//alarma en modo irq
isl1208INT.LPMODE=1;//deshabilita comunicacion en modo backup
ISL1208_Set_Int(&isl1208INT.Valor);
}

void isl1208_set_time(unsigned char hr, unsigned char min, unsigned char sec) {

#ifndef USE_INTERRUPTS
    //di();
#endif

    sec &= 0x7F;
    min &= 0x7F;
    hr &= 0x3F;
    start_i2c();
    write_i2c(isl1208_Write);
    write_i2c(0x07);
    write_i2c(0b00010000);
    stop_i2c();
    start_i2c();
    write_i2c(isl1208_Write);
    write_i2c(0x00);
    write_i2c(isl1208_bin2bcd(sec));
    write_i2c(isl1208_bin2bcd(min));
    write_i2c(isl1208_bin2bcd(hr) | 0b10000000);
    stop_i2c();

#ifndef USE_INTERRUPTS
    //  ei();
#endif

}

void isl1208_set_date(unsigned char* day, unsigned char* mth, unsigned char* year, unsigned char* dow) {

#ifndef USE_INTERRUPTS
    //di();
#endif

    start_i2c();
    write_i2c(isl1208_Write);
    write_i2c(0x07);
    write_i2c(0b00010000);
    stop_i2c();
    start_i2c();
    write_i2c(isl1208_Write);
    write_i2c(0x03);
    write_i2c(isl1208_bin2bcd(*day));
    write_i2c(isl1208_bin2bcd(*mth));
    write_i2c(isl1208_bin2bcd(*year));
    write_i2c(isl1208_bin2bcd(*dow));
    stop_i2c();

#ifndef USE_INTERRUPTS
    //  ei();
#endif

}

void isl1208_get_date(unsigned char* day, unsigned char* mth, unsigned char* year, unsigned char *dow) {

#ifndef USE_INTERRUPTS
    // di();
#endif

    start_i2c();
    write_i2c(isl1208_Write);
    write_i2c(0x03);
    rstart_i2c();
    write_i2c(isl1208_Read);
    *day = isl1208_bcd2bin(read_i2c(ACK) & 0x3f);
    *mth = isl1208_bcd2bin(read_i2c(ACK) & 0x1f);
    *year = isl1208_bcd2bin(read_i2c(ACK));
    *dow = isl1208_bcd2bin(read_i2c(NOACK) & 0x07);
    stop_i2c();
#ifndef USE_INTERRUPTS
    // ei();
#endif

}

void isl1208_get_time(unsigned char *hr, unsigned char* min, unsigned char *sec) {

#ifndef USE_INTERRUPTS
    // di();
#endif

    start_i2c();
    write_i2c(isl1208_Write);
    write_i2c(0x00);

    rstart_i2c();
    write_i2c(isl1208_Read);
    *sec = isl1208_bcd2bin(read_i2c(ACK) & 0x7f);
    * min = isl1208_bcd2bin(read_i2c(ACK) & 0x7f);
    * hr = isl1208_bcd2bin(read_i2c(NOACK) & 0x3f);
    stop_i2c();

#ifndef USE_INTERRUPTS
    // ei();
#endif

}

void isl1208_set_time_enc(unsigned char hr, unsigned char min, unsigned char sec) {
#ifndef USE_INTERRUPTS
    //di();
#endif
    min &=0x7F;
    hr &= 0x3F;
    
    start_i2c();
    write_i2c(isl1208_Write);
    write_i2c(0x0C);
    write_i2c(isl1208_bin2bcd(0));
    write_i2c(isl1208_bin2bcd(min)| 0b10000000); //habilita la alarma de minutos
    write_i2c(isl1208_bin2bcd(hr) | 0b10000000); //habilita la alarma de horas
    stop_i2c();

#ifndef USE_INTERRUPTS
    //  ei();
#endif

}
void isl1208_set_dow_enc(unsigned char *dow) {
//TODO terminar de implementar la funcion de escritura de dow.
#ifndef USE_INTERRUPTS
    //di();
#endif
    *dow &=0x07;
    start_i2c();
    write_i2c(isl1208_Write);
    write_i2c(0x11);
    write_i2c(isl1208_bin2bcd(*dow)| 0b10000000); //habilita la alarma de dow
    stop_i2c();

#ifndef USE_INTERRUPTS
    //  ei();
#endif

}

void isl1208_get_dow_enc(unsigned char *dow) {
//TODO terminar de implementar la funcion de escritura de dow.
#ifndef USE_INTERRUPTS
    //di();
#endif

    start_i2c();
    write_i2c(isl1208_Write);
    write_i2c(0x11);
    rstart_i2c();
    write_i2c(isl1208_Read);
    *dow = isl1208_bcd2bin(read_i2c(NOACK) & 0x07);
    stop_i2c();

#ifndef USE_INTERRUPTS
    //  ei();
#endif

}
void isl1208_get_time_enc(unsigned char *hr, unsigned char* min, unsigned char *sec) {

#ifndef USE_INTERRUPTS
    // di();
#endif

    start_i2c();
    write_i2c(isl1208_Write);
    write_i2c(0x0C);

    rstart_i2c();
    write_i2c(isl1208_Read);
    *sec = isl1208_bcd2bin(read_i2c(ACK) & 0x7f);
    * min = isl1208_bcd2bin(read_i2c(ACK) & 0x7f);
    * hr = isl1208_bcd2bin(read_i2c(NOACK) & 0x3f);
    stop_i2c();

#ifndef USE_INTERRUPTS
    // ei();
#endif

}

char isl1208_read_nvram_byte(char addr) {

    char retval;

#ifndef USE_INTERRUPTS
    // di();
#endif

    start_i2c();
    write_i2c(isl1208_Write);
    write_i2c(addr);

    start_i2c();
    write_i2c(isl1208_Read);
    retval = read_i2c(NOACK);
    stop_i2c();



#ifndef USE_INTERRUPTS
    // ei();
#endif
    return (retval);
}

void isl1208_write_nvram_byte(char addr, char value) {

#ifndef USE_INTERRUPTS
    //di();
#endif

    start_i2c();
    write_i2c(isl1208_Write);
    write_i2c(addr);
    write_i2c(value);
    stop_i2c();

#ifndef USE_INTERRUPTS
    // ei();
#endif

}
int aniobisiesto(unsigned char year)
{
    return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}
unsigned char dia_de_la_semana(unsigned char *dia, unsigned char *mes, unsigned char *anio){
//se usa el algoritmo de Zeller
   unsigned char a,y,m,dow;

   a = (14 -*mes) / 12;
y = *anio - a;
m = *mes + 12 * a - 2;

//Para el calendario Juliano:
//d = (5 + dia + y + y/4 + (31*m)/12) mod 7

//Para el calendario Gregoriano:
 dow = (*dia + y + y/4 - y/100 + y/400 + (31*m)/12) % 7;

//El resultado es un cero (0) para el domingo, 1 para el lunes? 6 para el sábado

 return (dow);
}
///////////////////////////////////////////////////////////////////////////////

unsigned char isl1208_bin2bcd(unsigned char binary_value) {
    return ((binary_value / 10) << 4) +binary_value % 10;

}

unsigned char isl1208_bcd2bin(unsigned char bcd_value) {
    return (bcd_value & 0x0F) + (bcd_value >> 4) * 10;
}


/*unsigned int ISL1208::bcd2bin(unsigned char val)
{
    return (val & 0x0F) + (val >> 4) * 10;
}

char ISL1208::bin2bcd(unsigned int val)
{
    return ((val / 10) << 4) + val % 10;
}*/
///////////////////////////////////////////////////////////////////////////////




