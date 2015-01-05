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

#ifndef ISL1208_H
#define ISL1208_H

#include "mbed.h"

/** ISL1208 class.
 *  Used for controlling an ISL1208 real time clock connected via I2C.
 *
 * Example:
 * @code
 * #include "mbed.h"
 * #include "ISL1208.h"
 *
 * ISL1208 rtc(p28, p27);
 *
 * int main()
 * {
 *     //Try to open the ISL1208
 *     if (rtc.open()) {
 *         printf("Device detected!\n");
 *
 *         //Configure the oscillator for a 32.768kHz crystal
 *         rtc.oscillatorMode(ISL1208::OSCILLATOR_CRYSTAL);
 *
 *         //Check if we need to reset the time
 *         if (rtc.powerFailed()) {
 *             //The time has been lost due to a power complete power failure
 *             printf("Device has lost power! Resetting time...\n");
 *
 *             //Set RTC time to Wed, 28 Oct 2009 11:35:37
 *             rtc.time(1256729737);
 *         }
 *
 *         while(1) {
 *             //Get the current time
 *             time_t seconds = rtc.time();
 *
 *             //Print the time in various formats
 *             printf("\nTime as seconds since January 1, 1970 = %d\n", seconds);
 *             printf("Time as a basic string = %s", ctime(&seconds));
 *             char buffer[32];
 *             strftime(buffer, 32, "%I:%M %p\n", localtime(&seconds));
 *             printf("Time as a custom formatted string = %s", buffer);
 *
 *             //Delay for 1.0 seconds
 *             wait(1.0);
 *         }
 *     } else {
 *         error("Device not detected!\n");
 *     }
 * }
 * @endcode
 */
class ISL1208
{
public:
    /** Represents the different oscillator modes for the ISL1208
     */
    enum OscillatorMode {
        OSCILLATOR_CRYSTAL,  /**< The internal crystal oscillator is enabled */
        OSCILLATOR_EXTERNAL  /**< The internal crystal oscillator is disabled, an external 32kHz oscillator is connected to X1 */
    };

    /** Represents the different output frequencies for the IRQ/fOUT pin on the ISL1208
     */
    enum OutputFrequency {
        FOUT_DISABLED = 0,  /**< Disable frequency generation on the IRQ/fOUT pin */
        FOUT_32768_HZ,      /**< Generate 32.768kHz on the IRQ/fOUT pin */
        FOUT_4096_HZ,       /**< Generate 4.096kHz on the IRQ/fOUT pin */
        FOUT_1024_HZ,       /**< Generate 1.024kHz on the IRQ/fOUT pin */
        FOUT_64_HZ,         /**< Generate 64Hz on the IRQ/fOUT pin */
        FOUT_32_HZ,         /**< Generate 32Hz on the IRQ/fOUT pin */
        FOUT_16_HZ,         /**< Generate 16Hz on the IRQ/fOUT pin */
        FOUT_8_HZ,          /**< Generate 8Hz on the IRQ/fOUT pin */
        FOUT_4_HZ,          /**< Generate 4Hz on the IRQ/fOUT pin */
        FOUT_2_HZ,          /**< Generate 2Hz on the IRQ/fOUT pin */
        FOUT_1_HZ,          /**< Generate 1Hz on the IRQ/fOUT pin */
        FOUT_1_2_HZ,        /**< Generate 1/2Hz on the IRQ/fOUT pin */
        FOUT_1_4_HZ,        /**< Generate 1/4Hz on the IRQ/fOUT pin */
        FOUT_1_8_HZ,        /**< Generate 1/8Hz on the IRQ/fOUT pin */
        FOUT_1_16_HZ,       /**< Generate 1/16Hz on the IRQ/fOUT pin */
        FOUT_1_32_HZ        /**< Generate 1/32Hz on the IRQ/fOUT pin */
    };

    /** Represents the power mode of the ISL1208
     */
    enum PowerMode {
        POWER_NORMAL,   /**< VBAT supply will be used when VDD < VBAT - VBATHYS and VDD < VTRIP */
        POWER_LPMODE    /**< VBAT supply will be used when VDD < VBAT - VBATHYS */
    };

    /** Represents the alarm mode of the ISL1208
     */
    enum AlarmMode {
        ALARM_DISABLED, /**< The alarm function is disabled */
        ALARM_SINGLE,   /**< Only a one-time match will be made between the RTC and alarm registers, and the IRQ/fOUT pin will pull low until the alarm flag is cleared */
        ALARM_INTERRUPT /**< Every time an alarm match is made the IRQ/fOUT pin will pull low for 250ms, the alarm flag does not have to be cleared */
    };

    /** Represents the different battery mode ATR selections of the ISL1208
     */
    enum BatteryModeATR {
        BMATR_ZERO = 0,     /**< Delta capacitance = 0pF */
        BMATR_MINUS_0_5,    /**< Delta capacitance = -0.5pF (≈ +2ppm) */
        BMATR_PLUS_0_5,     /**< Delta capacitance = +0.5pF (≈ -2ppm) */
        BMATR_PLUS_1_0      /**< Delta capacitance = +1pF (≈ -4ppm) */
    };

    /** Represents the different digital trim selections of the ISL1208
     */
    enum DigitalTrim {
        DTR_ZERO = 0,       /**< Digital trim = 0ppm */
        DTR_PLUS_20,        /**< Digital trim = ≈ +20ppm */
        DTR_PLUS_40,        /**< Digital trim = ≈ +40ppm */
        DTR_PLUS_60,        /**< Digital trim = ≈ +60ppm */
        DTR_MINUS_20 = 5,   /**< Digital trim = ≈ -20ppm */
        DTR_MINUS_40,       /**< Digital trim = ≈ -40ppm */
        DTR_MINUS_60        /**< Digital trim = ≈ -60ppm */
    };

    /** Create a ISL1208 object connected to the specified I2C pins
     *
     * @param sda The I2C data pin.
     * @param scl The I2C clock pin.
     * @param hz The I2C bus frequency (defaults to 400kHz).
     */
    ISL1208(PinName sda, PinName scl, int hz = 400000);

    /** Probe for the ISL1208 and configure auto reset if present
     *
     * @returns
     *   'true' if the device exists on the bus,
     *   'false' if the device doesn't exist on the bus.
     */
    bool open();

    /** Get the current time from the ISL1208
     *
     * @returns The current time as a Unix timestamp.
     */
    time_t time();

    /** Set the current time on the ISL1208 from a Unix timestamp
     *
     * @param t The current time as a Unix timestamp.
     */
    void time(time_t t);

    /** Determine whether the RTC has completely lost power (and hence the time and SRAM)
     *
     * @returns Whether or not the RTC has completely lost power.
     */
    bool powerFailed();

    /** Determine whether the RTC has been on battery backup
     *
     * @returns Whether or not the RTC has been on battery backup.
     */
    bool batteryFlag();

    /** Clear the battery backup flag
     */
    void clearBatteryFlag();

    /** Determine whether the alarm has matched the RTC
     *
     * @returns Whether or not the alarm has matched the RTC.
     */
    bool alarmFlag();

    /** Clear the alarm flag
     */
    void clearAlarmFlag();

    /** Get the current oscillator mode of the ISL1208
     *
     * @returns The current oscillator mode as a OscillatorMode enum.
     */
    ISL1208::OscillatorMode oscillatorMode();

    /** Set the oscillator mode of the ISL1208
     *
     * @param mode The new oscillator mode as a OscillatorMode enum.
     */
    void oscillatorMode(OscillatorMode mode);

    /** Get the current output frequency on the IRQ/fOUT pin
     *
     * @returns The current output frequency.
     */
    ISL1208::OutputFrequency foutFrequency();

    /** Set the output frequency on the IRQ/fOUT pin
     *
     * @param freq The new output frequency.
     */
    void foutFrequency(OutputFrequency freq);

    /** Determine whether or not the IRQ/fOUT pin will continue to output on battery power
     *
     * @returns Whether or not the IRQ/fOUT pin will continue to output on battery power.
     */
    bool outputOnBattery();

    /** Set whether or not the IRQ/fOUT pin should continue to output on battery power
     *
     * @param output Whether or not the IRQ/fOUT pin should continue to output on battery power.
     */
    void outputOnBattery(bool output);

    /** Get the current power mode of the ISL1208
     *
     * @returns The current power mode as a PowerMode enum.
     */
    ISL1208::PowerMode powerMode();

    /** Set the power mode of the ISL1208
     *
     * @param mode The new power mode as a PowerMode enum.
     */
    void powerMode(PowerMode mode);

    /** Get the current alarm mode of the ISL1208
     *
     * @returns The current alarm mode as an AlarmMode enum.
     */
    ISL1208::AlarmMode alarmMode();

    /** Set the alarm mode of the ISL1208
     *
     * @param mode The new alarm mode as an AlarmMode enum.
     */
    void alarmMode(AlarmMode mode);

    /** Get the current analog trim of the ISL1208
     *
     * @returns The current analog trim in pF.
     */
    float analogTrim();

    /** Set the analog trim of the ISL1208
     *
     * @param trim The new analog trim in pF (valid range 4.5pF to 20.25pF in 0.25pF steps).
     */
    void analogTrim(float trim);

    /** Get the current battery mode analog trim of the ISL1208
     *
     * @returns The current battery mode analog trim as a BatteryModeATR enum.
     */
    ISL1208::BatteryModeATR batteryModeATR();

    /** Set the battery mode analog trim of the ISL1208
     *
     * @param mode The new battery mode analog trim as a BatteryModeATR enum.
     */
    void batteryModeATR(BatteryModeATR atr);

    /** Get the current digital trim of the ISL1208
     *
     * @returns The current digital trim as a DigitalTrim enum.
     */
    ISL1208::DigitalTrim digitalTrim();

    /** Set the digital trim of the ISL1208
     *
     * @param mode The new digital trim as a DigitalTrim enum.
     */
    void digitalTrim(DigitalTrim dtr);

    /** Get the alarm time from the ISL1208
     *
     * @returns The current alarm time as a Unix timestamp (with the current year)
     */
    time_t alarmTime();

    /** Set the alarm time on the ISL1208 from a Unix timestamp
     *
     * @param t The alarm time as a Unix timestamp.
     * @param sc Enable or disable seconds matching.
     * @param mn Enable or disable minutes matching.
     * @param hr Enable or disable hours matching.
     * @param dt Enable or disable date matching.
     * @param mo Enable or disable month matching.
     * @param dw Enable or disable day of week matching.
     */
    void alarmTime(time_t t, bool sc = true, bool mn = true, bool hr = true, bool dt = true, bool mo = true, bool dw = true);

    /** Read the contents of the battery-backed SRAM
     *
     * @returns The current contents of the SRAM.
     */
    unsigned short sram();

    /** Write new data to the battery-backed SRAM
     *
     * @param data The new data for the SRAM.
     */
    void sram(unsigned short data);

private:
    //I2C register addresses
    enum Register {
        REG_RTC_SC  = 0x00,
        REG_RTC_MN,
        REG_RTC_HR,
        REG_RTC_DT,
        REG_RTC_MO,
        REG_RTC_YR,
        REG_RTC_DW,
        REG_CTL_SR,
        REG_CTL_INT,
        REG_CTL_ATR = 0x0A,
        REG_CTL_DTR,
        REG_ALM_SCA,
        REG_ALM_MNA,
        REG_ALM_HRA,
        REG_ALM_DTA,
        REG_ALM_MOA,
        REG_ALM_DWA,
        REG_USR_USR1,
        REG_USR_USR2
    };

    //Member constants
    static const int m_ADDR;

    //Member variables
    I2C m_I2C;

    //Internal functions
    char read8(char reg);
    void write8(char reg, char data);
    unsigned short read16(char reg);
    void write16(char reg, unsigned short data);
    unsigned int bcd2bin(unsigned char val);
    char bin2bcd(unsigned int val);
};

#endif
