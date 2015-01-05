///////////////////////////////////////////////////////////////////////////////////////
///                               DS1307.C                                           ///
///                     Driver for Real Time Clock                                   ///
///                     modified by Redpic 08/2006                                   ///
///                  http://picmania.garcia-cuervo.com                               ///
///                                                                                  ///
/// void ds1307_init(val)                                                            ///
///                  - Enable oscillator without clearing the seconds register       ///
///                    used when PIC loses power and DS1307 run from 3V BAT          ///
///                  - Config Control Register with next parameters:                 ///
///                     DS1307_ALL_DISABLED          All disabled                    ///
///                     DS1307_OUT_ON_DISABLED_HIHG  Out to Hight on Disable Out     ///
///                     DS1307_OUT_ENABLED           Out Enabled                     ///
///                     DS1307_OUT_1_HZ              Freq. Out to 1 Hz               ///
///                     DS1307_OUT_4_KHZ             Freq. Out to 4.096 Khz          ///
///                     DS1307_OUT_8_KHZ             Freq. Out to 8.192 Khz          ///
///                     DS1307_OUT_32_KHZ            Freq. Out to 32.768 Khz         ///
///                                                                                  ///
///                     Example init:                                                ///
///                     ds1307_init(DS1307_ALL_DISABLED);                            ///
///                     ds1307_init(DS1307_OUT_ENABLED | DS1307_OUT_1_HZ);           ///
///                                                                                  ///
/// void ds1307_set_date_time(day,mth,year,dow,hour,min,sec) - Set the date/time     ///
///                                                                                  ///
/// void ds1307_get_date(day,mth,year,dow)                   - Get the date          ///
///                                                                                  ///
/// void ds1307_get_time(hr,min,sec)                         - Get the time          ///
///                                                                                  ///
/// char ds1307_read_nvram_unsigned char(char addr)                   - Read unsigned char in address  ///
///                                                                                  ///
/// void ds1307_write_nvram_unsigned char(char addr, char value)      - Write unsigned char in address ///
///                                                                                  ///
/// void ds1307_get_day_of_week(char* ptr)                   - Get string Day Of Week///
///                                                                                  ///
/// If defined USE_INTERRUPTS all functions disable Global Interrupts on starts and  ///
///                           enable Global on ends else usar can do it hiself       ///
///                                                                                  ///
///////////////////////////////////////////////////////////////////////////////////////

#include <xc.h>
#include <stdio.h>
#include <stdbool.h>       /* For true/false definition */
#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "HardI2C.h"
#include "isl1208.h"

//la identificacion del isl1208 es 0xDE para escribir y 0XDF para leer
void isl1208_init(unsigned char val){

   unsigned char seconds = 0;

#ifndef USE_INTERRUPTS
	//di();
#endif

   start_i2c();
  I2Cstate=write_i2c(0xD0);
  write_i2c(0x00);
   rstart_i2c();
  write_i2c(0xD1);
   seconds = ds1307_bcd2bin(read_i2c(NOACK));
   stop_i2c();
   seconds &= 0x7F;

   __delay_us(3);

    start_i2c();
   write_i2c(0xD0);
   write_i2c(0x00);
   write_i2c(ds1307_bin2bcd(seconds));
   stop_i2c();
   start_i2c();
   write_i2c(0xD0);
   write_i2c(0x07);
   write_i2c(val);
   stop_i2c();

#ifndef USE_INTERRUPTS
	//ei();
#endif

}

void ds1307_set_date_time(unsigned char day, unsigned char mth, unsigned char year, unsigned char dow, unsigned char hr, unsigned char min, unsigned char sec){

#ifndef USE_INTERRUPTS
  //di();
#endif

  sec &= 0x7F;
  hr &= 0x3F;

  start_i2c();
  I2Cstate=write_i2c(0xD0);
  write_i2c(0x00);
  write_i2c(ds1307_bin2bcd(sec));
  write_i2c(ds1307_bin2bcd(min));
  write_i2c(ds1307_bin2bcd(hr));
  write_i2c(ds1307_bin2bcd(dow));
  write_i2c(ds1307_bin2bcd(day));
  write_i2c(ds1307_bin2bcd(mth));
  write_i2c(ds1307_bin2bcd(year));
  stop_i2c();

#ifndef USE_INTERRUPTS
 //  ei();
#endif

}

void ds1307_get_date(unsigned char* day, unsigned char* mth, unsigned char* year, unsigned char *dow){

#ifndef USE_INTERRUPTS
  // di();
#endif

  start_i2c();
  I2Cstate=write_i2c(0xD0);
  write_i2c(0x03);
  
  rstart_i2c();
  write_i2c(0xD1);
  *dow = ds1307_bcd2bin(read_i2c(ACK) & 0x07);
  *day  = ds1307_bcd2bin(read_i2c(ACK) & 0x3f);
  *mth  = ds1307_bcd2bin(read_i2c(ACK) & 0x1f);
  *year = ds1307_bcd2bin(read_i2c(NOACK));
stop_i2c();
#ifndef USE_INTERRUPTS
 // ei();
#endif

}

void ds1307_get_time(unsigned char *hr, unsigned char* min, unsigned char *sec){

#ifndef USE_INTERRUPTS
  // di();
#endif

  start_i2c();
  I2Cstate=write_i2c(0xD0);
  write_i2c(0x00);
 
  rstart_i2c();
  write_i2c(0xD1);
*sec = ds1307_bcd2bin(read_i2c(ACK) & 0x7f);
* min = ds1307_bcd2bin(read_i2c(ACK) & 0x7f);
* hr  = ds1307_bcd2bin(read_i2c(NOACK) & 0x3f);
  stop_i2c();

#ifndef USE_INTERRUPTS
  // ei();
#endif

}


char ds1307_read_nvram_byte (char addr){

   char retval;

#ifndef USE_INTERRUPTS
  // di();
#endif

   start_i2c();
   I2Cstate=write_i2c(0xD0);
   write_i2c(addr);
  
   rstart_i2c();
   write_i2c(0xD1);
   retval = read_i2c(NOACK);
   stop_i2c();

   

#ifndef USE_INTERRUPTS
  // ei();
#endif
return(retval);
}

void ds1307_write_nvram_byte (char addr, char value){

#ifndef USE_INTERRUPTS
   //di();
#endif

   start_i2c();
   I2Cstate=write_i2c(0xD0);
   write_i2c(addr);
   write_i2c(value);
   stop_i2c();

#ifndef USE_INTERRUPTS
  // ei();
#endif

}

void ds1307_get_day_of_week(char* ptr){

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
   ds1307_get_date(plday,plmonth,plyr,pldow);
   sprintf(ptr,"%s",days_of_week[*pldow-1]);
}

///////////////////////////////////////////////////////////////////////////////

unsigned char ds1307_bin2bcd(unsigned char binary_value){
    return ((binary_value / 10) << 4) + binary_value % 10;

/*unsigned char temp;
  unsigned char retval;

  temp = binary_value;
  retval = 0;
  while(1){
    if(temp >= 10){
      temp -= 10;
      retval += 0x10;
    }else{
      retval += temp;
      break;
    }
  }
  return(retval);*/
}

unsigned char ds1307_bcd2bin(unsigned char bcd_value){
    return (bcd_value & 0x0F) + (bcd_value >> 4) * 10;
  /*unsigned char temp;

  temp = bcd_value;
  temp >>= 1;
  temp &= 0x78;
  return(temp + (temp >> 2) + (bcd_value & 0x0f));*/
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




