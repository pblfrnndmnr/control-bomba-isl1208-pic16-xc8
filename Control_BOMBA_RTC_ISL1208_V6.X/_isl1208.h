#ifndef	_isl1208_H_
#define	_isl1208_H_
///////////////////////////////////////////////////////////////////////////////////////
///                               isl1208.C                                          ///
///                     Driver for Real Time Clock                                   ///
///                     modified by Pablo Fernando Manieri 1/2015                    ///
///                                                                                  ///
///                                                                                  ///
/// void isl1208_init(val)                                                           ///
///                  - Enable oscillator without clearing the seconds register       ///
///                    used when PIC loses power and isl1208 run from 3V BAT          ///
///                  - Config Control Register with next parameters:                 ///
///                     isl1208_ALL_DISABLED          All disabled                    ///
///                     isl1208_OUT_ON_DISABLED_HIHG  Out to Hight on Disable Out     ///
///                     isl1208_OUT_ENABLED           Out Enabled                     ///
///                     isl1208_OUT_1_HZ              Freq. Out to 1 Hz               ///
///                     isl1208_OUT_4_KHZ             Freq. Out to 4.096 Khz          ///
///                     isl1208_OUT_8_KHZ             Freq. Out to 8.192 Khz          ///
///                     isl1208_OUT_32_KHZ            Freq. Out to 32.768 Khz         ///
///                                                                                  ///
///                     Example init:                                                ///
///                     isl1208_init(isl1208_ALL_DISABLED);                            ///
///                     isl1208_init(isl1208_OUT_ENABLED | isl1208_OUT_1_HZ);           ///
///                                                                                  ///
/// void isl1208_set_date_time(day,mth,year,dow,hour,min,sec) - Set the date/time     ///
///                                                                                  ///
/// void isl1208_get_date(day,mth,year,dow)                   - Get the date          ///
///                                                                                  ///
/// void isl1208_get_time(hr,min,sec)                         - Get the time          ///
///                                                                                  ///
/// char isl1208_read_nvram_unsigned char(char addr)                   - Read unsigned char in address  ///
///                                                                                  ///
/// void isl1208_write_nvram_unsigned char(char addr, char value)      - Write unsigned char in address ///
///                                                                                  ///
/// void isl1208_get_day_of_week(char* ptr)                   - Get string Day Of Week///
///                                                                                  ///
/// If defined USE_INTERRUPTS all functions disable Global Interrupts on starts and  ///
///                           enable Global on ends else usar can do it hiself       ///
///                                                                                  ///
///////////////////////////////////////////////////////////////////////////////////////

#ifndef RTC_SDA
#define RTC_SDA  PIN_B0
#define RTC_SCL  PIN_B1
#endif

// ISL1208 Internal REGISTERS
#define ISL_SEC      0x00
#define ISL_MIN      0x01
#define ISL_HOUR     0x02
#define ISL_DAY      0x03
#define ISL_MONTH    0x04
#define ISL_YEAR     0x05
#define ISL_DAYWEEK  0x06
#define ISL_STATUS   0x07
#define ISL_INTERRUPT      0x08
#define ISL_ANALOGUE_TRIM  0x0A
#define ISL_DIGITAL_TRIM   0x0B
#define ISL_SEC_ALARM      0x0C
#define ISL_MIN_ALARM      0x0D
#define ISL_HOUR_ALARM     0x0E
#define ISL_DAY_ALARM      0x0F
#define ISL_MONTH_ALARM    0x10
#define ISL_DAYWEEK_ALARM  0x11
#define ISL_USER_RAM_1     0x12
#define ISL_USER_RAM_2     0x13

// ISL1208 Internal REGISTERS MASKS
#define MASK_SEC 0x7F
#define MASK_MIN 0x7F
#define MASK_HOUR 0x3F
#define MASK_DAY  0x3F
#define MASK_MONTH 0x1F
#define MASK_YEAR 0xFF

// ISL1208 Internal Bits masks
#define ISL_RTCF  0x01
#define ISL_BAT  0x02
#define ISL_ALM   0x04
#define ISL_WRTC  0x10

#define ISL_12_24HR 0x80

#define ISL_FOBATB    0x10
#define ISL_FO_32768  0x01
#define ISL_FO_4095   0x02
#define ISL_FO_1024   0x03
#define ISL_FO_64     0x04
#define ISL_FO_32     0x05
#define ISL_FO_16     0x06
#define ISL_FO_8      0x07
#define ISL_FO_4      0x08
#define ISL_FO_2      0x09
#define ISL_FO_1      0x0A
#define ISL_FO_0      0x00


#define  isl1208_Write 0xDE
#define  isl1208_Read 0xDF




const unsigned char days_of_week[7][3] = {"Do\0", "Lu\0", "Ma\0", "Mi\0", "Ju\0", "Vi\0", "Sa\0"};
const unsigned char cadena_esp[]="        ";
bool ISL1208_ready(void);
unsigned char isl1208_bin2bcd(unsigned char binary_value);
unsigned char isl1208_bcd2bin(unsigned char bcd_value);

void isl1208_init();

void isl1208_set_time(unsigned char hr, unsigned char min, unsigned char sec);
void isl1208_set_date(unsigned char* day, unsigned char* mth, unsigned char* year, unsigned char* dow);

void isl1208_get_date(unsigned char * day, unsigned char *mth, unsigned char *year, unsigned char *dow);
void isl1208_get_time(unsigned char *hr, unsigned char *min, unsigned char *sec);

void isl1208_set_time_enc(unsigned char hr, unsigned char min, unsigned char sec);
void isl1208_set_dow_enc(unsigned char *dow);

void isl1208_get_dow_enc(unsigned char *dow);
void isl1208_get_time_enc(unsigned char *hr, unsigned char *min, unsigned char *sec);

char isl1208_read_nvram_byte(char addr);
void isl1208_write_nvram_byte(char addr, char value);
int aniobisiesto(unsigned char year);
unsigned char dia_de_la_semana(unsigned char *dia, unsigned char *mes, unsigned char *anio);
unsigned char ISL1208_Read_status(void);
void ISL1208_Set_status(unsigned char *SR);
unsigned char ISL1208_Read_Int(void);
void ISL1208_Set_Int(unsigned char *Int);
///////////////////////////////////////////////////////////////////////////////

#endif
