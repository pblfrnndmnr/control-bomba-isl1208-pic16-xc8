/* ISL1208 Driver Library
 * Copyright (c) 2013 Neil Thiessen
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ISL1208.h"

const int ISL1208::m_ADDR = (0x6F << 1);

ISL1208::ISL1208(PinName sda, PinName scl, int hz) : m_I2C(sda, scl)
{
    //Set the I2C bus frequency
    m_I2C.frequency(hz);
}

bool ISL1208::open()
{
    //Probe for the ISL1208 using a Zero Length Transfer
    if (!m_I2C.write(m_ADDR, NULL, 0)) {
        //Read the current status register
        char sr = read8(REG_CTL_SR);

        //Disable auto reset for BAT and ALM bits
        sr &= ~(1 << 7);

        //Write the new status register
        write8(REG_CTL_SR, sr);

        //Return success
        return true;
    } else {
        //Return failure
        return false;
    }
}

time_t ISL1208::time()
{
    //Setup a tm structure based on the RTC
    struct tm timeinfo;
    timeinfo.tm_sec = bcd2bin(read8(REG_RTC_SC));
    timeinfo.tm_min = bcd2bin(read8(REG_RTC_MN));

    //Make sure we get the proper hour regardless of the mode
    char hours = read8(REG_RTC_HR);
    if (hours & (1 << 7)) {
        //RTC is in 24-hour mode
        timeinfo.tm_hour = bcd2bin(hours & 0x3F);
    } else {
        //RTC is in 12-hour mode
        timeinfo.tm_hour = bcd2bin(hours & 0x1F);

        //Check for the PM flag
        if (hours & (1 << 5))
            timeinfo.tm_hour += 12;
    }

    //Continue reading the registers
    timeinfo.tm_mday = bcd2bin(read8(REG_RTC_DT));
    timeinfo.tm_mon = bcd2bin(read8(REG_RTC_MO)) - 1;
    timeinfo.tm_year = bcd2bin(read8(REG_RTC_YR)) + 100;
    timeinfo.tm_wday = bcd2bin(read8(REG_RTC_DW));

    //Return as a timestamp
    return mktime(&timeinfo);
}

void ISL1208::time(time_t t)
{
    //Convert the time to a tm
    struct tm *timeinfo = localtime(&t);

    /* The clock has an 8 bit wide bcd-coded register (they never learn)
     * for the year. tm_year is an offset from 1900 and we are interested
     * in the 2000-2099 range, so any value less than 100 is invalid.
     */
    if (timeinfo->tm_year < 100)
        return;

    //Read the old SR register value
    char sr = read8(REG_CTL_SR);

    //Enable RTC writing
    write8(REG_CTL_SR, sr | (1 << 4));

    //Write the current time
    write8(REG_RTC_SC, bin2bcd(timeinfo->tm_sec));
    write8(REG_RTC_MN, bin2bcd(timeinfo->tm_min));
    write8(REG_RTC_HR, bin2bcd(timeinfo->tm_hour) | (1 << 7));    //24-hour mode
    write8(REG_RTC_DT, bin2bcd(timeinfo->tm_mday));
    write8(REG_RTC_MO, bin2bcd(timeinfo->tm_mon + 1));
    write8(REG_RTC_YR, bin2bcd(timeinfo->tm_year - 100));
    write8(REG_RTC_DW, bin2bcd(timeinfo->tm_wday & 7));

    //Disable RTC writing
    write8(REG_CTL_SR, sr);
}

bool ISL1208::powerFailed()
{
    //Read the 8-bit register value
    char value = read8(REG_CTL_SR);

    //Return the status of the RTCF bit
    if (value & (1 << 0))
        return true;
    else
        return false;
}

bool ISL1208::batteryFlag()
{
    //Read the 8-bit register value
    char value = read8(REG_CTL_SR);

    //Return the status of the BAT bit
    if (value & (1 << 1))
        return true;
    else
        return false;
}

void ISL1208::clearBatteryFlag()
{
    //Read the current 8-bit register value
    char value = read8(REG_CTL_SR);

    //Clear the BAT bit
    value &= ~(1 << 1);

    //Write the value back out
    write8(REG_CTL_SR, value);
}

bool ISL1208::alarmFlag()
{
    //Read the 8-bit register value
    char value = read8(REG_CTL_SR);

    //Return the status of the ALM bit
    if (value & (1 << 2))
        return true;
    else
        return false;
}

void ISL1208::clearAlarmFlag()
{
    //Read the current 8-bit register value
    char value = read8(REG_CTL_SR);

    //Clear the ALM bit
    value &= ~(1 << 2);

    //Write the value back out
    write8(REG_CTL_SR, value);
}

ISL1208::OscillatorMode ISL1208::oscillatorMode()
{
    //Read the 8-bit register value
    char value = read8(REG_CTL_SR);

    //Return the status of the XTOSCB bit
    if (value & (1 << 6))
        return OSCILLATOR_EXTERNAL;
    else
        return OSCILLATOR_CRYSTAL;
}

void ISL1208::oscillatorMode(OscillatorMode mode)
{
    //Read the current 8-bit register value
    char value = read8(REG_CTL_SR);

    //Set or clear the XTOSCB bit
    if (mode == OSCILLATOR_EXTERNAL)
        value |= (1 << 6);
    else
        value &= ~(1 << 6);

    //Write the value back out
    write8(REG_CTL_SR, value);
}

ISL1208::OutputFrequency ISL1208::foutFrequency()
{
    //Read the 8-bit register value
    char value = read8(REG_CTL_INT);

    //Return the lower nibble
    return (OutputFrequency)(value & 0x0F);
}

void ISL1208::foutFrequency(OutputFrequency freq)
{
    //Read the current 8-bit register value
    char value = read8(REG_CTL_INT);

    //Clear the old frequency bits
    value &= 0xF0;

    //Set the new frequency bits
    value |= freq;

    //Write the value back out
    write8(REG_CTL_INT, value);
}

bool ISL1208::outputOnBattery()
{
    //Read the 8-bit register value
    char value = read8(REG_CTL_INT);

    //Return the status of the FOBATB bit
    if (value & (1 << 4))
        return false;
    else
        return true;
}

void ISL1208::outputOnBattery(bool output)
{
    //Read the current 8-bit register value
    char value = read8(REG_CTL_INT);

    //Set or clear the FOBATB bit
    if (output)
        value &= ~(1 << 4);
    else
        value |= (1 << 4);

    //Write the value back out
    write8(REG_CTL_INT, value);
}

ISL1208::PowerMode ISL1208::powerMode()
{
    //Read the 8-bit register value
    char value = read8(REG_CTL_INT);

    //Return the status of the LPMODE bit
    if (value & (1 << 5))
        return POWER_LPMODE;
    else
        return POWER_NORMAL;
}

void ISL1208::powerMode(PowerMode mode)
{
    //Read the current 8-bit register value
    char value = read8(REG_CTL_INT);

    //Set or clear the LPMODE bit
    if (mode == POWER_LPMODE)
        value |= (1 << 5);
    else
        value &= ~(1 << 5);

    //Write the value back out
    write8(REG_CTL_INT, value);
}

ISL1208::AlarmMode ISL1208::alarmMode()
{
    //Read the 8-bit register value
    char value = read8(REG_CTL_INT);

    //Return the status of the ALME and IM bits
    if (value & (1 << 6)) {
        if (value & (1 << 7))
            return ALARM_INTERRUPT;
        else
            return ALARM_SINGLE;
    } else
        return ALARM_DISABLED;
}

void ISL1208::alarmMode(AlarmMode mode)
{
    //Read the current 8-bit register value
    char value = read8(REG_CTL_INT);

    //Set or clear the ALME and IM bit
    if (mode != ALARM_DISABLED) {
        value |= (1 << 6);
        if (mode == ALARM_INTERRUPT)
            value |= (1 << 7);
        else
            value &= ~(1 << 7);
    } else
        value &= ~(1 << 6);

    //Write the value back out
    write8(REG_CTL_INT, value);
}

float ISL1208::analogTrim()
{
    //Read the 8-bit register value
    char value = read8(REG_CTL_ATR);

    //Mask off the top 2 bits
    value &= 0x3F;

    //Invert bit 5
    value ^= 1 << 5;

    //Add an offset of 4.5pF (unit[atr] = 0.25pF)
    value += 2 * 9;

    //Return the analog trim in pF
    return value * 0.25;
}

void ISL1208::analogTrim(float trim)
{
    //Range limit trim
    if (trim < 4.5)
        trim = 4.5;
    else if (trim > 20.25)
        trim = 20.25;

    //Convert the analog trim value to a 6-bit integer
    char value = (char)(trim / 0.25);

    //Remove the offset of 4.5pF (unit[atr] = 0.25pF)
    value -= 2 * 9;

    //Invert bit 5
    value ^= 1 << 5;

    //Read the current 8-bit register value
    char reg = read8(REG_CTL_ATR);

    //Clear the old ATR bits
    reg &= 0xC0;

    //Add the new ATR bits
    reg |= value;

    //Write the value back out
    write8(REG_CTL_ATR, reg);
}

ISL1208::BatteryModeATR ISL1208::batteryModeATR()
{
    //Read the 8-bit register value
    char value = read8(REG_CTL_ATR);

    //Shift out the ATR bits
    value >>= 6;

    //Return the value as a BatteryModeATR enum
    return (BatteryModeATR)value;
}

void ISL1208::batteryModeATR(BatteryModeATR atr)
{
    //Read the current 8-bit register value
    char value = read8(REG_CTL_ATR);

    //Clear the old battery mode ATR bits
    value &= 0x3F;

    //Add the new battery mode ATR bits
    value |= (atr << 6);

    //Write the value back out
    write8(REG_CTL_ATR, value);
}

ISL1208::DigitalTrim ISL1208::digitalTrim()
{
    //Read the 8-bit register value
    char value = read8(REG_CTL_DTR);

    //Mask off the reserved bit
    value &= ~(1 << 7);

    //Return the value as a DigitalTrim enum
    return (DigitalTrim)value;
}

void ISL1208::digitalTrim(DigitalTrim dtr)
{
    //Read the current 8-bit register value (to preserve the reserved bit)
    char value = read8(REG_CTL_DTR);

    //Clear the old DTR bits
    value &= 0xF8;

    //Add the new DTR bits
    value |= dtr;

    //Write the value back out
    write8(REG_CTL_DTR, value);
}

time_t ISL1208::alarmTime()
{
    //Setup a tm structure based on the RTC
    struct tm timeinfo;

    //MSB of each alarm register is an enable bit
    timeinfo.tm_sec = bcd2bin(read8(REG_ALM_SCA) & 0x7F);
    timeinfo.tm_min = bcd2bin(read8(REG_ALM_MNA) & 0x7F);
    timeinfo.tm_hour = bcd2bin(read8(REG_ALM_HRA) & 0x3F);
    timeinfo.tm_mday = bcd2bin(read8(REG_ALM_DTA) & 0x3F);
    timeinfo.tm_mon = bcd2bin(read8(REG_ALM_MOA) & 0x1F) - 1;
    timeinfo.tm_wday = bcd2bin(read8(REG_ALM_DWA) & 0x03);

    //The alarm doesn't store the year, so get it from the RTC section
    timeinfo.tm_year = bcd2bin(read8(REG_RTC_YR)) + 100;

    //Return as a timestamp
    return mktime(&timeinfo);
}

void ISL1208::alarmTime(time_t t, bool sc, bool mn, bool hr, bool dt, bool mo, bool dw)
{
    //Convert the time to a tm
    struct tm *timeinfo = localtime(&t);

    //Write the new alarm time components (if enabled)
    if (sc)
        write8(REG_ALM_SCA, bin2bcd(timeinfo->tm_sec) | 0x80);
    else
        write8(REG_ALM_SCA, 0x0);
    if (mn)
        write8(REG_ALM_MNA, bin2bcd(timeinfo->tm_min) | 0x80);
    else
        write8(REG_ALM_MNA, 0x0);
    if (hr)
        write8(REG_ALM_HRA, bin2bcd(timeinfo->tm_hour) | 0x80);
    else
        write8(REG_ALM_HRA, 0x0);
    if (hr)
        write8(REG_ALM_DTA, bin2bcd(timeinfo->tm_mday) | 0x80);
    else
        write8(REG_ALM_DTA, 0x0);
    if (mo)
        write8(REG_ALM_MOA, bin2bcd(timeinfo->tm_mon + 1) | 0x80);
    else
        write8(REG_ALM_MOA, 0x0);
    if (dw)
        write8(REG_ALM_DWA, bin2bcd(timeinfo->tm_wday & 7) | 0x80);
    else
        write8(REG_ALM_DWA, 0x0);
}

unsigned short ISL1208::sram()
{
    //Return the complete contents of the SRAM
    return read16(REG_USR_USR1);
}

void ISL1208::sram(unsigned short data)
{
    //Write the complete contents of the SRAM
    write16(REG_USR_USR1, data);
}

char ISL1208::read8(char reg)
{
    //Select the register
    m_I2C.write(m_ADDR, &reg, 1, true);

    //Read the 8-bit register
    m_I2C.read(m_ADDR, &reg, 1);

    //Return the byte
    return reg;
}

void ISL1208::write8(char reg, char data)
{
    //Create a temporary buffer
    char buff[2];

    //Load the register address and 8-bit data
    buff[0] = reg;
    buff[1] = data;

    //Write the data
    m_I2C.write(m_ADDR, buff, 2);
}

unsigned short ISL1208::read16(char reg)
{
    //Create a temporary buffer
    char buff[2];

    //Select the register
    m_I2C.write(m_ADDR, &reg, 1, true);

    //Read the 16-bit register
    m_I2C.read(m_ADDR, buff, 2);

    //Return the combined 16-bit value
    return (buff[0] << 8) | buff[1];
}

void ISL1208::write16(char reg, unsigned short data)
{
    //Create a temporary buffer
    char buff[3];

    //Load the register address and 16-bit data
    buff[0] = reg;
    buff[1] = data >> 8;
    buff[2] = data;

    //Write the data
    m_I2C.write(m_ADDR, buff, 3);
}

unsigned int ISL1208::bcd2bin(unsigned char val)
{
    return (val & 0x0F) + (val >> 4) * 10;
}

char ISL1208::bin2bcd(unsigned int val)
{
    return ((val / 10) << 4) + val % 10;
}
