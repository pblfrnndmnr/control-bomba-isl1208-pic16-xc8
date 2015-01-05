			/****************************************************************************
 *
 * $COPYRIGHT_START$
 * (C) COPYRIGHT  
 * $COPYRIGHT_END$
 *
 * Filename: ISL1208_RTC_V1.c
 *
 * Original Author: 
 *
 * Revision: $Header: $
 *
 * Description: Functions for driving ISL1208 Real Time Clock Device
 *
 ****************************************************************************/

/****************************************************************************
 * Include files
 ****************************************************************************/

/****************************************************************************
 * Definitions
 ****************************************************************************/
#define ISL1208_I2C_WRITE  0xDE
#define ISL1208_I2C_READ   0xDF

// ISL1208 Internal REGISTERS
#define ISL_SEC      0x00
#define ISL_MIN           0x01
#define ISL_HOUR        0x02
#define ISL_DAY         0x03
#define ISL_MONTH   0x04
#define ISL_YEAR   0x05
#define ISL_DAYWEEK   0x06
#define ISL_STATUS   0x07
#define ISL_INTERRUPT   0x08
#define ISL_ANALOGUE_TRIM   0x0A
#define ISL_DIGITAL_TRIM   0x0B
#define ISL_SEC_ALARM   0x0C
#define ISL_MIN_ALARM   0x0D
#define ISL_HOUR_ALARM   0x0E
#define ISL_DAY_ALARM   0x0F
#define ISL_MONTH_ALARM   0x10
#define ISL_DAYWEEK_ALARM   0x11
#define ISL_USER_RAM_1   0x12
#define ISL_USER_RAM_2   0x13

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

#define ISL_FOBATB   0x10
#define ISL_FO_32768  0x01
#define ISL_FO_4095  0x02
#define ISL_FO_1024  0x03
#define ISL_FO_64  0x04
#define ISL_FO_32  0x05
#define ISL_FO_16  0x06
#define ISL_FO_8  0x07
#define ISL_FO_4  0x08
#define ISL_FO_2  0x09
#define ISL_FO_1  0x0A
#define ISL_FO_0  0x00

/****************************************************************************
 * Enumerated types and structure definitions
 ****************************************************************************/
 
/****************************************************************************
* Global variables
****************************************************************************/


/****************************************************************************
* Local variables (declared static)
****************************************************************************/

/****************************************************************************
 * Function prototypes
 ***************************************************************************/
BOOLEAN ISL1208_ready(void);
void ISL1208_write(unsigned char DeviceWriteAddr, unsigned char DeviceReg, unsigned char in_data);
int ISL1208_read(unsigned int DeviceWriteAddr, unsigned int DeviceReg, unsigned int DeviceReadAddr);
void ISL1208_init(void);

int16 convertMinutes(unsigned int now_hour, unsigned int now_min);
int read_RTCandConvert(int read_reg, int reg_mask);
void write_RTCandConvert(int write_reg, int time_value, int reg_mask);

/*****************************************************************************
* Function:  BOOLEAN ISL1208_ready(void)
* Description: Check that the device is ready on the bus
*              The CCS i2C_Write function returns the ACK Bit.
*              0 means ACK, 
*              1 means NO ACK
*              2 means there was a collision if in Multi_Master Mode
* Parameters:  NONE
* Return:      BOOLEAN TRUE = Device OK, FALSE = Device FAIL
* Globals:     NONE
*
******************************************************************************/
BOOLEAN ISL1208_ready(void)
{
   int1 ack;
   
   i2c_start();            // If the write command is acknowledged,
   ack = i2c_write(ISL1208_I2C_WRITE);  // then the device is ready.
   i2c_stop();
   return !ack;     // Invert ACK returned so TRUE = ACK OK
}


/*****************************************************************************
* Function: void ISL1208_write(unsigned char DeviceWriteAddr, unsigned char DeviceReg, unsigned char in_data)
* Description: Write to the ISL1208 RTC device in the I2C bus.
* Parameters:  unsigned char DeviceWriteAddr = Device Bus Address
*              unsigned char DeviceReg = Register to write to (Use #defines)
*              unsigned char in_data = Data to write to the register
* Return:      NONE
* Globals:     NONE
******************************************************************************/
void ISL1208_write(unsigned char DeviceWriteAddr, unsigned char DeviceReg, unsigned char in_data)
{
   while(!ISL1208_ready());
   i2c_start();                   // start condition 
   i2c_write(DeviceWriteAddr);    // Send slave address and clear (W only)
   i2c_write(DeviceReg);           // Command byte and register to be written.
   i2c_write(in_data);           // Send data
   i2c_stop();                   // stop condition
}

/*****************************************************************************
* Function: long int ISL1208_read(unsigned int DeviceWriteAddr, unsigned int DeviceReg, unsigned int DeviceReadAddr)
* Description: Read the ISL1208 RTC device in the I2C bus.
* Parameters:  unsigned char DeviceWriteAddr = Device Bus Address
*              unsigned char DeviceReg = Register to Read (Use #defines)
*              unsigned int DeviceReadAddr = Device Bus Address Read
* Return:      NONE
* Globals:     NONE
*
******************************************************************************/
int ISL1208_read(unsigned int DeviceWriteAddr, unsigned int DeviceReg, unsigned int DeviceReadAddr)
{
   unsigned int data;

   while(!ISL1208_ready());
   i2c_start();                     // start condition
   i2c_write(DeviceWriteAddr);      // Send slave address and set (W) 
   i2c_write(DeviceReg);            // Command byte and register to be read. 
   i2c_start();  
   i2c_write(DeviceReadAddr);       // Send slave address and set (R)  
   data = i2c_read(0);              // Data from LSB or MSB of register and NAK
   i2c_stop();                      // stop condition

   return(data);
}


/*****************************************************************************
* Function:  void ISL1208_init(void)
* Description: Initialise the ISL1208 RTC device on the bus. A fixed setup for the
*              given project.
* Parameters:  NONE
* Return:      NONE
* Globals:     NONE
*
******************************************************************************/
void ISL1208_init(void)
{
   // Set-up selected RTC Clock 
   ISL1208_write(ISL1208_I2C_WRITE, ISL_STATUS, ISL_WRTC);      /* Enable the Write */
   ISL1208_write(ISL1208_I2C_WRITE, ISL_INTERRUPT, (ISL_FOBATB |ISL_FO_32768)); /* 32KHz OUT */
   ISL1208_write(ISL1208_I2C_WRITE, ISL_HOUR, ISL_12_24HR); /* 24 hour mode */   
   
   ISL1208_write(ISL1208_I2C_WRITE, ISL_SEC, 0x00);
   ISL1208_write(ISL1208_I2C_WRITE, ISL_MIN, 0x00);
   ISL1208_write(ISL1208_I2C_WRITE, ISL_HOUR, 0x8D); /* 13.00 hrs, 24 hour mode */
}

/*****************************************************************************
* Function:  convertMinutes(void)
* Description: Convert the passed in time to minutes only
* Parameters: unsigned int now_hour - hours to convert
*             unsigned int now_min - minutes to convert
* Return: int16 of current minutes
* Globals: NONE
*
******************************************************************************/
int16 convertMinutes(unsigned int now_hour, unsigned int now_min)
{
   int16 total_minutes;

   total_minutes = ((now_hour * 60) + now_min);
   return total_minutes;   
}

/*****************************************************************************
* Function: int read_RTCandConvert(int read_reg)
* Description: Convert the RTC registers to a printable number
* Parameters: int read_reg: Register that requires access
*           int reg_mask: Mask for the bits in the register to extract number
* Return: int value convereted
* Globals: NONE
******************************************************************************/
int read_RTCandConvert(int read_reg, int reg_mask)
{
   int temp_value, out_value;
   
   temp_value = (ISL1208_read(ISL1208_I2C_WRITE, read_reg, ISL1208_I2C_READ)& reg_mask);

   out_value = temp_value & 0x0F;   /* mask out the seconds */
   out_value = out_value + ((temp_value >> 4)* 10);
   return out_value;
}

/*****************************************************************************
* Function: int write_RTCandConvert(int write_reg, int time_value)
* Description: Convert the value to format for RTC registers.
* Parameters: int write_reg: Register that requires access
*           int time_value: value of time to be converted and written
*           int reg_mask: Register mask for that written to.
* Return:  NONE
* Globals: NONE
******************************************************************************/
void write_RTCandConvert(int write_reg, int time_value, int reg_mask)
{
   int temp_value = 0;

   temp_value = (time_value/10)<<4;
   temp_value |= (time_value%10);
   temp_value &= reg_mask;
   if(write_reg == ISL_HOUR)
   {
      temp_value |= ISL_12_24HR; /* Add in the 24 hour mode bit */
   }
   ISL1208_write(ISL1208_I2C_WRITE, write_reg, temp_value);  
}