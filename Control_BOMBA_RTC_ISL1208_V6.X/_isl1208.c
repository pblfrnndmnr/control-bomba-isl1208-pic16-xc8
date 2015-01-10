////////////////////////////////////////////////////////////////////////////////////////
///                               isl1208.C                                          ///
///                     Driver for Real Time Clock                                   ///
///                     modified by Pablo Fernando Manieri - Enero 2015              ///
///                                                                                  ///
/// bool ISL1208_ready(void) - Testea si el RTC est� presente                        ///
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

bool ISL1208_ready(void)
{
   bool ack;
   
   start_i2c();            // If the write command is acknowledged,
   ack = write_i2c(isl1208_Write);  // then the device is ready.
   stop_i2c();
   return !ack;     // Invert ACK returned so TRUE = ACK OK
}

unsigned char ISL1208_Read_status(void)
{
   char SR;
  start_i2c();            // If the write command is acknowledged,
   write_i2c(isl1208_Write);  // then the device is ready.
   write_i2c(0x07);
   start_i2c();
   write_i2c(isl1208_Read);
   SR=read_i2c(NOACK);
   stop_i2c();
   return SR;     
}

void isl1208_init(unsigned char val){

   unsigned char seconds = 0;

#ifndef USE_INTERRUPTS
	//di();
#endif
//TODO Verificar secuencia correcta para pode escribir en el ISL1208
   start_i2c();
  I2Cstate=write_i2c(isl1208_Write);
  write_i2c(0x00);
   rstart_i2c();
  write_i2c(isl1208_Read);
   seconds = isl1208_bcd2bin(read_i2c(NOACK));
   stop_i2c();
   seconds &= 0x7F;

   __delay_us(3);

    start_i2c();
   write_i2c(isl1208_Write);
   write_i2c(0x00);
   write_i2c(isl1208_bin2bcd(seconds));
   stop_i2c();
   start_i2c();
   write_i2c(isl1208_Write);
   write_i2c(0x07);
   write_i2c(val);
   stop_i2c();

#ifndef USE_INTERRUPTS
	//ei();
#endif

}

void isl1208_set_date_time(unsigned char day, unsigned char mth, unsigned char year, unsigned char dow, unsigned char hr, unsigned char min, unsigned char sec){

#ifndef USE_INTERRUPTS
  //di();
#endif

  sec &= 0x7F;
  hr &= 0x3F;

  start_i2c();
  I2Cstate=write_i2c(isl1208_Write);
  write_i2c(0x00);
  write_i2c(isl1208_bin2bcd(sec));
  write_i2c(isl1208_bin2bcd(min));
  write_i2c(isl1208_bin2bcd(hr));
  write_i2c(isl1208_bin2bcd(dow));
  write_i2c(isl1208_bin2bcd(day));
  write_i2c(isl1208_bin2bcd(mth));
  write_i2c(isl1208_bin2bcd(year));
  stop_i2c();

#ifndef USE_INTERRUPTS
 //  ei();
#endif

}

void isl1208_get_date(unsigned char* day, unsigned char* mth, unsigned char* year, unsigned char *dow){

#ifndef USE_INTERRUPTS
  // di();
#endif

  start_i2c();
  I2Cstate=write_i2c(isl1208_Write);
  write_i2c(0x03);
  start_i2c();
  write_i2c(isl1208_Read);
  *dow = isl1208_bcd2bin(read_i2c(ACK) & 0x07);
  *day  = isl1208_bcd2bin(read_i2c(ACK) & 0x3f);
  *mth  = isl1208_bcd2bin(read_i2c(ACK) & 0x1f);
  *year = isl1208_bcd2bin(read_i2c(NOACK));
stop_i2c();
#ifndef USE_INTERRUPTS
 // ei();
#endif

}

void isl1208_get_time(unsigned char *hr, unsigned char* min, unsigned char *sec){

#ifndef USE_INTERRUPTS
  // di();
#endif

  start_i2c();
  I2Cstate=write_i2c(isl1208_Write);
  write_i2c(0x00);
 
  start_i2c();
  write_i2c(isl1208_Read);
*sec = isl1208_bcd2bin(read_i2c(ACK) & 0x7f);
* min = isl1208_bcd2bin(read_i2c(ACK) & 0x7f);
* hr  = isl1208_bcd2bin(read_i2c(NOACK) & 0x3f);
  stop_i2c();

#ifndef USE_INTERRUPTS
  // ei();
#endif

}


char isl1208_read_nvram_byte (char addr){

   char retval;

#ifndef USE_INTERRUPTS
  // di();
#endif

   start_i2c();
   I2Cstate=write_i2c(isl1208_Write);
   write_i2c(addr);
  
   start_i2c();
   write_i2c(isl1208_Read);
   retval = read_i2c(NOACK);
   stop_i2c();

   

#ifndef USE_INTERRUPTS
  // ei();
#endif
return(retval);
}

void isl1208_write_nvram_byte (char addr, char value){

#ifndef USE_INTERRUPTS
   //di();
#endif

   start_i2c();
   I2Cstate=write_i2c(isl1208_Write);
   write_i2c(addr);
   write_i2c(value);
   stop_i2c();

#ifndef USE_INTERRUPTS
  // ei();
#endif

}

void isl1208_get_day_of_week(char* ptr){

   unsigned char *plday;
   unsigned char *plmonth;
   unsigned char *plyr;
   unsigned char *pldow;
   unsigned char lday=0;
   unsigned char lmonth=0;
   unsigned char lyr=0;
   unsigned char ldow=0;
   plday=&lday;
   plmonth=&lmonth;
   plyr=&lyr;
   pldow=&ldow;
   isl1208_get_date(plday,plmonth,plyr,pldow);
   sprintf(ptr,"%s",days_of_week[*pldow-1]);
}

///////////////////////////////////////////////////////////////////////////////

unsigned char isl1208_bin2bcd(unsigned char binary_value){
    return ((binary_value / 10) << 4) + binary_value % 10;

}

unsigned char isl1208_bcd2bin(unsigned char bcd_value){
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




