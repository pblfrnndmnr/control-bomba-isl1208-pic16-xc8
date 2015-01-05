

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <xc.h>
#include <delays.h>

#include "config.h"


///////////////////////////////////////////////////////////////
//
// PIC and layout specific defines
//

#define _XTAL_FREQ      (8000000L)

#define LCD_ET10_PORT   PORTB
#define LCD_ET10_LAT    LATB
#define LCD_ET10_TRIS   TRISB

#define MSSP_SCL_TRIS   TRISCbits.TRISC3
#define MSSP_SCL_LAT    LATCbits.LATC3
#define MSSP_SDA_TRIS   TRISCbits.TRISC4


///////////////////////////////////////////////////////////////
//
// 2x8 LCD Routines via ET10 cable to Port B
// using SPLC780D1 4-bit parallel protocol
//

#define LCD_BL      (0x01)
#define LCD_RS      (0x02)
#define LCD_RW      (0x04)
#define LCD_EN      (0x08)
#define LCD_DATA    (0xF0)

static uint8_t gLcdOut      = 0x00;


static void lcdSend()
{
    __delay_us( 1 );
    LCD_ET10_LAT = gLcdOut;
    __delay_us( 1 );
}


static void lcdPulseEN()
{
    __delay_us( 10 );
    gLcdOut |= LCD_EN;
    lcdSend();
    __delay_us( 10 );
    gLcdOut &= ~LCD_EN;
    lcdSend();
    __delay_us( 10 );
}


static uint8_t lcdGetNibble()
{
    uint8_t result = 0;
    lcdSend();
    __delay_us( 10 );
    gLcdOut |= LCD_EN;
    lcdSend();
    __delay_us( 10 );
    result = LCD_ET10_PORT & 0xF0;
    gLcdOut &= ~LCD_EN;
    lcdSend();
    __delay_us( 10 );
    return result;
}


static void lcdWaitBusy()
{
    // tri-state the data bits
    LCD_ET10_TRIS = 0xF0;
    // save the current out
    uint8_t save = gLcdOut;
    // set bits for IR Read
    gLcdOut |=  LCD_RW; // Read
    gLcdOut &= ~LCD_RS; // IR
    // read data until Busy bit cleared
    uint8_t data = 0;
    do {
        data = lcdGetNibble();
        data |= lcdGetNibble() >> 4;
    } while ( data & 0x80 );
    // reset data bits to output
    LCD_ET10_TRIS = 0x00;
    // restore current out
    gLcdOut = save;
}


static void lcdWriteIR( uint8_t byte )
{
    gLcdOut &= ~LCD_RW; // Write
    gLcdOut &= ~LCD_RS; // IR

    gLcdOut = ( gLcdOut & 0x0F ) | ( byte & 0xF0 );
    lcdPulseEN();
    lcdWaitBusy();

    gLcdOut = ( gLcdOut & 0x0F ) | ( ( byte & 0x0F ) << 4 );
    lcdPulseEN();
    lcdWaitBusy();
}


static void lcdWriteDR( uint8_t byte )
{
    gLcdOut &= ~LCD_RW; // Write
    gLcdOut |=  LCD_RS; // DR

    gLcdOut = ( gLcdOut & 0x0F ) | ( byte & 0xF0 );
    lcdPulseEN();
    lcdWaitBusy();

    gLcdOut = ( gLcdOut & 0x0F ) | ( ( byte & 0x0F ) << 4 );
    lcdPulseEN();
    lcdWaitBusy();
}


void lcdClear()
{
    // clear display
    lcdWriteIR( 0b00000001 );
    __delay_ms( 20 );
    lcdWaitBusy();
}


void lcdSetup()
{
    // set LCD ET10 port to digital output
    LCD_ET10_LAT = 0x00;
    LCD_ET10_TRIS = 0x00;

    // initialization sequence
    gLcdOut = 0b00110000;

    lcdPulseEN();
    __delay_ms( 20 );
    lcdPulseEN();
    __delay_ms( 20 );
    lcdPulseEN();
    __delay_ms( 20 );

    // set interface to 4-bit
    gLcdOut = 0b00100000;
    lcdPulseEN();
    lcdWaitBusy();

    // 4-bit, 2 lines, 5x8 font
    lcdWriteIR( 0b00101000 );

    // display on, cursor off
    lcdWriteIR( 0b00001100 );

    lcdClear();

    // entry mode r-to-l, no shift
    lcdWriteIR( 0b00000110 );

    // turn on backlight
    gLcdOut |= LCD_BL;
    lcdSend();

    // return home
    lcdWriteIR( 0b00000010 );
}


void lcdGoto( int col, int row )
{
    uint8_t address = col;
    if ( row ) address += 0x40;
    lcdWriteIR( 0x80 | address );
}


void lcdOut( const char *s )
{
    while ( *s )
        lcdWriteDR( *s++ );
}


///////////////////////////////////////////////////////////////
//
// I2C using MSSP
//

#define kI2C_MASTER     (0x01)
#define kI2C_DISABLE    (0x00)
#define kI2C_100KHZ     (0x00)
#define kI2C_400KHZ     (0x02)

#define BGP_BASE        (_XTAL_FREQ / 400000L)

void i2cSetup( uint8_t flags )
{
    if ( flags & kI2C_MASTER ) {
        // enable I2C module
        SSPCON1bits.SSPEN = 1;

        // I2C Master mode, clock = FOSC/(4 * (SSPADD + 1))
        SSPCON1bits.SSPM3 = 1;
        SSPCON1bits.SSPM2 = 0;
        SSPCON1bits.SSPM1 = 0;
        SSPCON1bits.SSPM0 = 0;

        // Calculate BGP from Fosc
        uint8_t bgp = (uint8_t) BGP_BASE;
        if ( flags & kI2C_400KHZ )
            bgp /= 4;
        bgp -= 1;
        SSPADD = bgp;

    } else {
        // disable I2C module
        SSPCON1bits.SSPEN = 0;
    }
}


int i2cXfer(
	uint8_t address,
	uint8_t* wdata, int wsize,
	uint8_t* rdata, int rsize )
{
    int count = 0;
    int hasWrite = ( wdata != NULL ) && ( wsize != 0 );
    int hasRead = ( rdata != NULL ) && ( rsize != 0 );
    if ( ! ( hasWrite || hasRead ) ) return -1;

    // generate start condition
    PIR1bits.SSPIF = 0;
    PIR2bits.BCLIF = 0;
    SSPCON2bits.SEN = 1;
    while ( 1 ) {
        if ( PIR2bits.BCLIF ) return -2;
        if ( PIR1bits.SSPIF ) break;
    }

    // Send 7-bit slave address and Read/Write flag
    PIR1bits.SSPIF = 0;
    PIR2bits.BCLIF = 0;
    SSPBUF = ( address << 1 ) | ( hasWrite ? 0x00 : 0x01 );
    while ( 1 ) {
        if ( PIR2bits.BCLIF ) return -3;
        if ( PIR1bits.SSPIF ) break;
    }
    if ( SSPCON2bits.ACKSTAT ) return -4;

    // Write data
    for ( int i = 0; i < wsize; i++ ) {
        PIR1bits.SSPIF = 0;
        PIR2bits.BCLIF = 0;
        SSPBUF = wdata[ i ];
        while ( 1 ) {
            if ( PIR2bits.BCLIF ) return -5;
            if ( PIR1bits.SSPIF ) break;
        }
        if ( SSPCON2bits.ACKSTAT ) return -6;
    }

    if ( hasRead ) {
        // if we've been writing, send a Repeated Start
        if ( hasWrite ) {
            // generate start condition
            PIR1bits.SSPIF = 0;
            PIR2bits.BCLIF = 0;
            SSPCON2bits.RSEN = 1;
            while ( 1 ) {
                if ( PIR2bits.BCLIF ) return -7;
                if ( PIR1bits.SSPIF ) break;
            }

            // Send 7-bit slave address with Read flag
            PIR1bits.SSPIF = 0;
            PIR2bits.BCLIF = 0;
            SSPBUF = ( address << 1 ) | 0x01;
            while ( 1 ) {
                if ( PIR2bits.BCLIF ) return -8;
                if ( PIR1bits.SSPIF ) break;
            }
            if ( SSPCON2bits.ACKSTAT ) return -9;
        }

        // Read data
        for ( int i = 0; i < rsize; i++ ) {
            // check idle
            if ( SSPCON2 & 0x1F ) return -10;

            // enable reception
            PIR1bits.SSPIF = 0;
            PIR2bits.BCLIF = 0;
            SSPCON2bits.RCEN = 1;
            while ( 1 ) {
                if ( PIR2bits.BCLIF ) return -11;
                if ( PIR1bits.SSPIF ) break;
            }

            // read data byte from buffer
            rdata[ i ] = SSPBUF;
            count++;

            // send acknowledge (or not acknowledge for final byte)
            PIR1bits.SSPIF = 0;
            PIR2bits.BCLIF = 0;
            SSPCON2bits.ACKDT = i == ( rsize - 1 ) ? 1 : 0;
            SSPCON2bits.ACKEN = 1;
            while ( 1 ) {
                if ( PIR2bits.BCLIF ) return -12;
                if ( PIR1bits.SSPIF ) break;
            }
        }
    }

    // Generate stop condition
    PIR1bits.SSPIF = 0;
    PIR2bits.BCLIF = 0;
    SSPCON2bits.PEN = 1;
    while ( 1 ) {
        if ( PIR2bits.BCLIF ) return -13;
        if ( PIR1bits.SSPIF ) break;
    }

    return count;
}


void i2cReset()
{
    // first make sure I2C module is disabled
    i2cSetup( kI2C_DISABLE );

    // hold SCL low for 100 ms to reset I2C bus
    MSSP_SCL_TRIS = 0;
    MSSP_SCL_LAT = 0;
    for ( int i = 0; i < 10; i++ )
        __delay_ms( 10 );
    // tri-date SCL and SDA
    MSSP_SCL_TRIS = 1;
    MSSP_SDA_TRIS = 1;
    __delay_ms( 10 );
}


///////////////////////////////////////////////////////////////
//
// Read time from MAXIM DS3232 Real-Time Clock using I2C
// protocol and print result to bottom line of LCD as HH:MM:SS
//


void clockUpdate()
{
    lcdGoto( 0, 1 );

    uint8_t wdata[1] = { 0x00 };
    uint8_t rdata[3] = { 0x00, 0x00, 0x00 };

    int result = i2cXfer( 0b1101000, wdata, 1, rdata, 3 );

    if ( result < 0 ) {
        char s[20] = { 0 };
        itoa( s, result, 10 );
        lcdOut( s );
        return;
    }

    char s[20] = { 0 };
    strcpy( s, "00:00:00" );
    int x = 8;
    for ( int i = 0; i < 3; i++ ) {
        char t[20] = { 0 };
        itoa( t, rdata[i], 16 );
        int k = strlen( t );
        memcpy( s + x - k, t, k );
        x -= 3;
    }

    lcdOut( s );
}


void main( void )
{
    OSCCONbits.IRCF = 0b1110;   // Internal RC Oscillator Frequency Select - HFINTOSC (8mHz)
    OSCCONbits.SCS  = 0b10;     // System Clock Select

    lcdSetup();
    lcdGoto( 0, 0 );
    lcdOut( "PIC16F1938 Clock" );

    i2cReset();
    i2cSetup( kI2C_MASTER | kI2C_400KHZ );

    while ( 1 ) {
        for ( int i = 0; i < 10; i++ )
            __delay_ms( 10 );
        clockUpdate();
    }
}

