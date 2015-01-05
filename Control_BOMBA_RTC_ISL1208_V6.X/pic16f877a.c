//Registros de configuracion del pic 16f877a
//Copiar y pegar en el codigo fuente  el que se necesite
#if 0


   // STATUS: REGISTER (ADDRESS 03h, 83h, 103h, 183h)
   STATUS = (unsigned char)
     +(0<<7) // IRP: Register Bank Select bit (used for indirect addressing)
             //   1 = Bank 2, 3 (100h-1FFh)
             //   0 = Bank 0, 1 (00h-FFh)
     +(0<<5) // RP: Register Bank Select bits (used for direct addressing)
             //   3 = Bank 3 (180h-1FFh)
             //   2 = Bank 2 (100h-17Fh)
             //   1 = Bank 1 (80h-FFh)
             //   0 = Bank 0 (00h-7Fh) Each bank is 128 bytes.
     +(0<<4) // TO: Time-out bit
             //   1 = After power-up, CLRWDT instruction or SLEEP instruction
             //   0 = A WDT time-out occurred
     +(0<<3) // PD: Power-down bit
             //   1 = After power-up or by the CLRWDT instruction
             //   0 = By execution of the SLEEP instruction
     +(0<<2) // Z: Zero bit
             //   1 = The result of an arithmetic or logic operation is zero
             //   0 = The result of an arithmetic or logic operation is not zero
     +(0<<1) // DC: Digit carry/borrow bit (ADDWF, ADDLW,SUBLW,SUBWF
             //       instructions) (for borrow, the polarity is reversed)
             //   1 = A carry-out from the 4th low order bit of the result
             //       occurred
             //   0 = No carry-out from the 4th low order bit of the result
     +(0);   // C: Carry/borrow bit (ADDWF, ADDLW,SUBLW,SUBWF instructions)
             //   1 = A carry-out from the Most Significant bit of the result
             //       occurred
             //   0 = No carry-out from the Most Significant bit of the result
             //       occurred



   // OPTION_REG: REGISTER (ADDRESS 81h, 181h)
   OPTION_REG = (unsigned char)
     +(0<<7) // RBPU: PORTB Pull-up Enable bit
             //   1 = PORTB pull-ups are disabled
             //   0 = PORTB pull-ups are enabled by individual port latch values
     +(0<<6) // INTEDG: Interrupt Edge Select bit
             //   1 = Interrupt on rising edge of RB0/INT pin
             //   0 = Interrupt on falling edge of RB0/INT pin
     +(0<<5) // T0CS: TMR0 Clock Source Select bit
             //   1 = Transition on RA4/T0CKI pin
             //   0 = Internal instruction cycle clock (CLKO)
     +(0<<4) // T0SE: TMR0 Source Edge Select bit
             //   1 = Increment on high-to-low transition on RA4/T0CKI pin
             //   0 = Increment on low-to-high transition on RA4/T0CKI pin
     +(0<<3) // PSA: Prescaler Assignment bit
             //   1 = Prescaler is assigned to the WDT
             //   0 = Prescaler is assigned to the Timer0 module
     +(0);   // PS: Prescaler Rate Select bits



   // INTCON: REGISTER (ADDRESS 0Bh, 8Bh, 10Bh, 18Bh)
   INTCON = (unsigned char)
     +(0<<7) // GIE: Global Interrupt Enable bit
             //   1 = Enables all unmasked interrupts
             //   0 = Disables all interrupts
     +(0<<6) // PEIE: Peripheral Interrupt Enable bit
             //   1 = Enables all unmasked peripheral interrupts
             //   0 = Disables all peripheral interrupts
     +(0<<5) // TMR0IE: TMR0 Overflow Interrupt Enable bit
             //   1 = Enables the TMR0 interrupt
             //   0 = Disables the TMR0 interrupt
     +(0<<4) // INTE: RB0/INT External Interrupt Enable bit
             //   1 = Enables the RB0/INT external interrupt
             //   0 = Disables the RB0/INT external interrupt
     +(0<<3) // RBIE: RB Port Change Interrupt Enable bit
             //   1 = Enables the RB port change interrupt
             //   0 = Disables the RB port change interrupt
     +(0<<2) // TMR0IF: TMR0 Overflow Interrupt Flag bit
             //   1 = TMR0 register has overflowed (must be cleared in software)
             //   0 = TMR0 register did not overflow
     +(0<<1) // INTF: RB0/INT External Interrupt Flag bit
             //   1 = The RB0/INT external interrupt occurred (must be cleared
             //       in software)
             //   0 = The RB0/INT external interrupt did not occur
     +(0);   // RBIF: RB Port Change Interrupt Flag bit
             //   1 = At least one of the RB7:RB4 pins changed state; a
             //       mismatch condition will continue to set the bit. Reading
             //       PORTB will end the mismatch condition and allow the bit
             //       to be cleared (must be cleared in software).
             //   0 = None of the RB7:RB4 pins have changed state



   // PIE1: REGISTER (ADDRESS 8Ch)
   PIE1 = (unsigned char)
     +(0<<7) // PSPIE: Parallel Slave Port Read/Write Interrupt Enable bit(1)
             //   1 = Enables the PSP read/write interrupt
             //   0 = Disables the PSP read/write interrupt
     +(0<<6) // ADIE: A/D Converter Interrupt Enable bit
             //   1 = Enables the A/D converter interrupt
             //   0 = Disables the A/D converter interrupt
     +(0<<5) // RCIE: USART Receive Interrupt Enable bit
             //   1 = Enables the USART receive interrupt
             //   0 = Disables the USART receive interrupt
     +(0<<4) // TXIE: USART Transmit Interrupt Enable bit
             //   1 = Enables the USART transmit interrupt
             //   0 = Disables the USART transmit interrupt
     +(0<<3) // SSPIE: Synchronous Serial Port Interrupt Enable bit
             //   1 = Enables the SSP interrupt
             //   0 = Disables the SSP interrupt
     +(0<<2) // CCP1IE: CCP1 Interrupt Enable bit
             //   1 = Enables the CCP1 interrupt
             //   0 = Disables the CCP1 interrupt
     +(0<<1) // TMR2IE: TMR2 to PR2 Match Interrupt Enable bit
             //   1 = Enables the TMR2 to PR2 match interrupt
             //   0 = Disables the TMR2 to PR2 match interrupt
     +(0);   // TMR1IE: TMR1 Overflow Interrupt Enable bit
             //   1 = Enables the TMR1 overflow interrupt
             //   0 = Disables the TMR1 overflow interrupt



   // PIR1: REGISTER (ADDRESS 0Ch)
   PIR1 = (unsigned char)
     +(0<<7) // PSPIF: Parallel Slave Port Read/Write Interrupt Flag bit(1)
             //   1 = A read or a write operation has taken place (must be
             //       cleared in software)
             //   0 = No read or write has occurred
     +(0<<6) // ADIF: A/D Converter Interrupt Flag bit
             //   1 = An A/D conversion completed
             //   0 = The A/D conversion is not complete
     +(0<<5) // RCIF: USART Receive Interrupt Flag bit
             //   1 = The USART receive buffer is full
             //   0 = The USART receive buffer is empty
     +(0<<4) // TXIF: USART Transmit Interrupt Flag bit
             //   1 = The USART transmit buffer is empty
             //   0 = The USART transmit buffer is full
     +(0<<3) // SSPIF: Synchronous Serial Port (SSP) Interrupt Flag bit
             //   1 = The SSP interrupt condition has occurred and must be
             //       cleared in software before returning from the Interrupt
             //       Service Routine. The conditions that will set this bit
             //       are: • SPI – A transmission/reception has taken
             //       place. • I2C Slave – A transmission/reception has
             //       taken place. • I2C Master - A transmission/reception
             //       has taken place. - The initiated Start condition was
             //       completed by the SSP module. - The initiated Stop
             //       condition was completed by the SSP module. - The
             //       initiated Restart condition was completed by the SSP
             //       module. - The initiated Acknowledge condition was
             //       completed by the SSP module. - A Start condition occurred
             //       while the SSP module was Idle (multi-master system). - A
             //       Stop condition occurred while the SSP module was Idle
             //       (multi-master system).
             //   0 = No SSP interrupt condition has occurred
     +(0<<2) // CCP1IF: CCP1 Interrupt Flag bit Capture mode:
             //   1 = A TMR1 register capture occurred (must be cleared in
             //       software)
             //   0 = No TMR1 register capture occurred Compare mode:
             //   1 = A TMR1 register compare match occurred (must be cleared
             //       in software)
             //   0 = No TMR1 register compare match occurred PWM mode: Unused
             //       in this mode.
     +(0<<1) // TMR2IF: TMR2 to PR2 Match Interrupt Flag bit
             //   1 = TMR2 to PR2 match occurred (must be cleared in software)
             //   0 = No TMR2 to PR2 match occurred
     +(0);   // TMR1IF: TMR1 Overflow Interrupt Flag bit
             //   1 = TMR1 register overflowed (must be cleared in software)
             //   0 = TMR1 register did not overflow



   // PIE2: REGISTER (ADDRESS 8Dh)
   PIE2 = (unsigned char)
     +(0<<7) // Unimplemented: Read as ‘0’
     +(0<<6) // CMIE: Comparator Interrupt Enable bit
             //   1 = Enables the comparator interrupt
             //   0 = Disable the comparator interrupt
     +(0<<5) // Unimplemented: Read as ‘0’
     +(0<<4) // EEIE: EEPROM Write Operation Interrupt Enable bit
             //   1 = Enable EEPROM write interrupt
             //   0 = Disable EEPROM write interrupt
     +(0<<3) // BCLIE: Bus Collision Interrupt Enable bit
             //   1 = Enable bus collision interrupt
             //   0 = Disable bus collision interrupt
     +(0<<1) // Unimplemented: Read as ‘0’
     +(0);   // CCP2IE: CCP2 Interrupt Enable bit
             //   1 = Enables the CCP2 interrupt
             //   0 = Disables the CCP2 interrupt



   // PIR2: REGISTER (ADDRESS 0Dh)
   PIR2 = (unsigned char)
     +(0<<7) // Unimplemented: Read as ‘0’
     +(0<<6) // CMIF: Comparator Interrupt Flag bit
             //   1 = The comparator input has changed (must be cleared in
             //       software)
             //   0 = The comparator input has not changed
     +(0<<5) // Unimplemented: Read as ‘0’
     +(0<<4) // EEIF: EEPROM Write Operation Interrupt Flag bit
             //   1 = The write operation completed (must be cleared in
             //       software)
             //   0 = The write operation is not complete or has not been
             //       started
     +(0<<3) // BCLIF: Bus Collision Interrupt Flag bit
             //   1 = A bus collision has occurred in the SSP when configured
             //       for I2 C Master mode
             //   0 = No bus collision has occurred
     +(0<<1) // Unimplemented: Read as ‘0’
     +(0);   // CCP2IF: CCP2 Interrupt Flag bit Capture mode:
             //   1 = A TMR1 register capture occurred (must be cleared in
             //       software)
             //   0 = No TMR1 register capture occurred Compare mode:
             //   1 = A TMR1 register compare match occurred (must be cleared
             //       in software)
             //   0 = No TMR1 register compare match occurred PWM mode: Unused.



   // PCON: REGISTER (ADDRESS 8Eh)
   PCON = (unsigned char)
     +(0<<2) // Unimplemented: Read as ‘0’
     +(0<<1) // POR: Power-on Reset Status bit
             //   1 = No Power-on Reset occurred
             //   0 = A Power-on Reset occurred (must be set in software after
             //       a Power-on Reset occurs)
     +(0);   // BOR: Brown-out Reset Status bit
             //   1 = No Brown-out Reset occurred
             //   0 = A Brown-out Reset occurred (must be set in software after
             //       a Brown-out Reset occurs)



   // EECON1: REGISTER (ADDRESS 18Ch)
   EECON1 = (unsigned char)
     +(0<<7) // EEPGD: Program/Data EEPROM Select bit
             //   1 = Accesses program memory
             //   0 = Accesses data memory Reads ‘0’ after a POR; this bit
             //       cannot be changed while a write operation is in progress.
     +(0<<4) // Unimplemented: Read as ‘0’
     +(0<<3) // WRERR: EEPROM Error Flag bit
             //   1 = A write operation is prematurely terminated (any MCLR or
             //       any WDT Reset during normal operation)
             //   0 = The write operation completed
     +(0<<2) // WREN: EEPROM Write Enable bit
             //   1 = Allows write cycles
             //   0 = Inhibits write to the EEPROM
     +(0<<1) // WR: Write Control bit
             //   1 = Initiates a write cycle. The bit is cleared by hardware
             //       once write is complete. The WR bit can only be set (not
             //       cleared) in software.
             //   0 = Write cycle to the EEPROM is complete
     +(0);   // RD: Read Control bit
             //   1 = Initiates an EEPROM read; RD is cleared in hardware. The
             //       RD bit can only be set (not cleared) in software.
             //   0 = Does not initiate an EEPROM read
     +(0<<3) // TTL Input/output pin or programming pin in LVP mode. Internal
             //       software programmable weak pull-up. RB4 bit 4 TTL
             //       Input/output pin (with interrupt-on-change). Internal
             //       software programmable weak pull-up. RB5 bit 5 TTL
             //       Input/output pin (with interrupt-on-change). Internal
             //       software programmable weak pull-up. RB6/PGC bit 6
             //       TTL/ST(2) Input/output pin (with interrupt-on-change) or
             //       in-circuit debugger pin. Internal software programmable
             //       weak pull-up. Serial programming clock. RB7/PGD bit 7
             //       TTL/ST(2) Input/output pin (with interrupt-on-change) or
             //       in-circuit debugger pin. Internal software programmable
             //       weak pull-up. Serial programming data.



   // TRISE: REGISTER (ADDRESS 89h)
   TRISE = (unsigned char)
     +(0<<7) // IBF: Input Buffer Full Status bit
             //   1 = A word has been received and is waiting to be read by the
             //       CPU
             //   0 = No word has been received
     +(0<<6) // OBF: Output Buffer Full Status bit
             //   1 = The output buffer still holds a previously written word
             //   0 = The output buffer has been read
     +(0<<5) // IBOV: Input Buffer Overflow Detect bit (in Microprocessor mode)
             //   1 = A write occurred when a previously input word has not
             //       been read (must be cleared in software)
             //   0 = No overflow occurred
     +(0<<4) // PSPMODE: Parallel Slave Port Mode Select bit
             //   1 = PORTD functions in Parallel Slave Port mode
             //   0 = PORTD functions in general purpose I/O mode
     +(0<<3) // Unimplemented: Read as ‘0’ PORTE Data Direction Bits:
     +(0<<2) // Bit 2: Direction Control bit for pin RE2/CS/AN7
             //   1 = Input
             //   0 = Output
     +(0<<1) // Bit 1: Direction Control bit for pin RE1/WR/AN6
             //   1 = Input
             //   0 = Output
     +(0);   // Bit 0: Direction Control bit for pin RE0/RD/AN5
             //   1 = Input
             //   0 = Output



   // OPTION_REG: REGISTER
   OPTION_REG = (unsigned char)
     +(0<<7) // 7 RBPU
     +(0<<6) // 6 INTEDG
     +(0<<5) // T0CS: TMR0 Clock Source Select bit
             //   1 = Transition on T0CKI pin
             //   0 = Internal instruction cycle clock (CLKO)
     +(0<<4) // T0SE: TMR0 Source Edge Select bit
             //   1 = Increment on high-to-low transition on T0CKI pin
             //   0 = Increment on low-to-high transition on T0CKI pin
     +(0<<3) // PSA: Prescaler Assignment bit
             //   1 = Prescaler is assigned to the WDT
             //   0 = Prescaler is assigned to the Timer0 module
     +(0);   // PS: Prescaler Rate Select bits



   // T1CON: TIMER1 CONTROL REGISTER (ADDRESS 10h)
   T1CON = (unsigned char)
     +(0<<6) // Unimplemented: Read as ‘0’
     +(0<<4) // T1CKPS: Timer1 Input Clock Prescale Select bits
             //   3 = 1:8 prescale value
             //   2 = 1:4 prescale value
             //   1 = 1:2 prescale value
             //   0 = 1:1 prescale value
     +(0<<3) // T1OSCEN: Timer1 Oscillator Enable Control bit
             //   1 = Oscillator is enabled
             //   0 = Oscillator is shut-off (the oscillator inverter is turned
             //       off to eliminate power drain)
     +(0<<2) // T1SYNC: Timer1 External Clock Input Synchronization Control bit
             //       When TMR1CS = 1:
             //   1 = Do not synchronize external clock input
             //   0 = Synchronize external clock input When TMR1CS = 0: This
             //       bit is ignored. Timer1 uses the internal clock when
             //       TMR1CS = 0.
     +(0<<1) // TMR1CS: Timer1 Clock Source Select bit
             //   1 = External clock from pin RC0/T1OSO/T1CKI (on the rising
             //       edge)
             //   0 = Internal clock (FOSC/4)
     +(0);   // TMR1ON: Timer1 On bit
             //   1 = Enables Timer1
             //   0 = Stops Timer1



   // T2CON: TIMER2 CONTROL REGISTER (ADDRESS 12h)
   T2CON = (unsigned char)
     +(0<<7) // Unimplemented: Read as ‘0’
     +(0<<3) // TOUTPS: Timer2 Output Postscale Select bits
             //   0 = 1:1 postscale
             //   1 = 1:2 postscale
             //   2 = 1:3 postscale • • •
             //   15 = 1:16 postscale
     +(0<<2) // TMR2ON: Timer2 On bit
             //   1 = Timer2 is on
             //   0 = Timer2 is off
     +(0);   // T2CKPS: Timer2 Clock Prescale Select bits
             //   0 = Prescaler is 1
             //   1 = Prescaler is 4
             //   1x = Prescaler is 16



   // CCP1CON: REGISTER/CCP2CON REGISTER (ADDRESS 17h/1Dh)
   CCP1CON = (unsigned char)
     +(0<<6) // Unimplemented: Read as ‘0’
     +(0<<4) // CCPxX:CCPxY: PWM Least Significant bits Capture mode: Unused.
             //       Compare mode: Unused. PWM mode: These bits are the two
             //       LSbs of the PWM duty cycle. The eight MSbs are found in
             //       CCPRxL.
     +(0);   // CCPxM: CCPx Mode Select bits
             //   0 = Capture/Compare/PWM disabled (resets CCPx module)
             //   4 = Capture mode, every falling edge
             //   5 = Capture mode, every rising edge
             //   6 = Capture mode, every 4th rising edge
             //   7 = Capture mode, every 16th rising edge
             //   8 = Compare mode, set output on match (CCPxIF bit is set)
             //   9 = Compare mode, clear output on match (CCPxIF bit is set)
             //   10 = Compare mode, generate software interrupt on match
             //       (CCPxIF bit is set, CCPx pin is unaffected)
             //   11 = Compare mode, trigger special event (CCPxIF bit is set,
             //       CCPx pin is unaffected); CCP1 resets TMR1; CCP2 resets
             //       TMR1 and starts an A/D conversion (if A/D module is
             //       enabled)
             //   3xx = PWM mode



   // SSPSTAT: MSSP STATUS REGISTER (SPI MODE) (ADDRESS 94h)
   SSPSTAT = (unsigned char)
     +(0<<7) // SMP: Sample bit SPI Master mode:
             //   1 = Input data sampled at end of data output time
             //   0 = Input data sampled at middle of data output time SPI
             //       Slave mode: SMP must be cleared when SPI is used in Slave
             //       mode.
     +(0<<6) // CKE: SPI Clock Select bit
             //   1 = Transmit occurs on transition from active to Idle clock
             //       state
             //   0 = Transmit occurs on transition from Idle to active clock
             //       state
     +(0<<5) // D/A: Data/Address bit Used in I2 C mode only.
     +(0<<4) // P: Stop bit Used in I2 C mode only. This bit is cleared when
             //       the MSSP module is disabled, SSPEN is cleared.
     +(0<<3) // S: Start bit Used in I2 C mode only.
     +(0<<2) // R/W: Read/Write bit information Used in I2C mode only.
     +(0<<1) // UA: Update Address bit Used in I2C mode only.
     +(0);   // BF: Buffer Full Status bit (Receive mode only)
             //   1 = Receive complete, SSPBUF is full
             //   0 = Receive not complete, SSPBUF is empty



   // SSPCON1: MSSP CONTROL REGISTER 1 (SPI MODE) (ADDRESS 14h)
   SSPCON1 = (unsigned char)
     +(0<<7) // WCOL: Write Collision Detect bit (Transmit mode only)
             //   1 = The SSPBUF register is written while it is still
             //       transmitting the previous word. (Must be cleared in
             //       software.)
             //   0 = No collision
     +(0<<6) // SSPOV: Receive Overflow Indicator bit SPI Slave mode:
             //   1 = A new byte is received while the SSPBUF register is still
             //       holding the previous data. In case of overflow, the data
             //       in SSPSR is lost. Overflow can only occur in Slave mode.
             //       The user must read the SSPBUF, even if only transmitting
             //       data, to avoid setting overflow. (Must be cleared in
             //       software.)
             //   0 = No overflow
     +(0<<5) // SSPEN: Synchronous Serial Port Enable bit
             //   1 = Enables serial port and configures SCK, SDO, SDI, and SS
             //       as serial port pins
             //   0 = Disables serial port and configures these pins as I/O
             //       port pins
     +(0<<4) // CKP: Clock Polarity Select bit
             //   1 = Idle state for clock is a high level
             //   0 = Idle state for clock is a low level
     +(0);   // SSPM: Synchronous Serial Port Mode Select bits
             //   5 = SPI Slave mode, clock = SCK pin. SS pin control disabled.
             //       SS can be used as I/O pin.
             //   4 = SPI Slave mode, clock = SCK pin. SS pin control enabled.
             //   3 = SPI Master mode, clock = TMR2 output/2
             //   2 = SPI Master mode, clock = FOSC/64
             //   1 = SPI Master mode, clock = FOSC/16
             //   0 = SPI Master mode, clock = FOSC/4



   // SSPSTAT: MSSP STATUS REGISTER (I2C MODE) (ADDRESS 94h)
   SSPSTAT = (unsigned char)
     +(0<<7) // SMP: Slew Rate Control bit In Master or Slave mode:
             //   1 = Slew rate control disabled for standard speed mode (100
             //       kHz and 1 MHz)
             //   0 = Slew rate control enabled for high-speed mode (400 kHz)
     +(0<<6) // CKE: SMBus Select bit In Master or Slave mode:
             //   1 = Enable SMBus specific inputs
             //   0 = Disable SMBus specific inputs
     +(0<<5) // D/A: Data/Address bit In Master mode: Reserved. In Slave mode:
             //   1 = Indicates that the last byte received or transmitted was
             //       data
             //   0 = Indicates that the last byte received or transmitted was
             //       address
     +(0<<4) // P: Stop bit
             //   1 = Indicates that a Stop bit has been detected last
             //   0 = Stop bit was not detected last
     +(0<<3) // S: Start bit
             //   1 = Indicates that a Start bit has been detected last
             //   0 = Start bit was not detected last
     +(0<<2) // R/W: Read/Write bit information (I2 C mode only) In Slave mode:
             //   1 = Read
             //   0 = Write
     +(0<<1) // UA: Update Address (10-bit Slave mode only)
             //   1 = Indicates that the user needs to update the address in
             //       the SSPADD register
             //   0 = Address does not need to be updated
     +(0);   // BF: Buffer Full Status bit In Transmit mode:
             //   1 = Receive complete, SSPBUF is full
             //   0 = Receive not complete, SSPBUF is empty In Receive mode:
             //   1 = Data Transmit in progress (does not include the ACK and
             //       Stop bits), SSPBUF is full
             //   0 = Data Transmit complete (does not include the ACK and Stop
             //       bits), SSPBUF is empty



   // SSPCON1: MSSP CONTROL REGISTER 1 (I2C MODE) (ADDRESS 14h)
   SSPCON1 = (unsigned char)
     +(0<<7) // WCOL: Write Collision Detect bit In Master Transmit mode:
             //   1 = A write to the SSPBUF register was attempted while the I2
             //       C conditions were not valid for a transmission to be
             //       started. (Must be cleared in software.)
             //   0 = No collision In Slave Transmit mode:
             //   1 = The SSPBUF register is written while it is still
             //       transmitting the previous word. (Must be cleared in
             //       software.)
             //   0 = No collision In Receive mode (Master or Slave modes):
             //       This is a “don’t care” bit.
     +(0<<6) // SSPOV: Receive Overflow Indicator bit In Receive mode:
             //   1 = A byte is received while the SSPBUF register is still
             //       holding the previous byte. (Must be cleared in software.)
             //   0 = No overflow In Transmit mode: This is a “don’t
             //       care” bit in Transmit mode.
     +(0<<5) // SSPEN: Synchronous Serial Port Enable bit
             //   1 = Enables the serial port and configures the SDA and SCL
             //       pins as the serial port pins
             //   0 = Disables the serial port and configures these pins as I/O
             //       port pins
     +(0<<4) // CKP: SCK Release Control bit In Slave mode:
             //   1 = Release clock
             //   0 = Holds clock low (clock stretch). (Used to ensure data
             //       setup time.) In Master mode: Unused in this mode.
     +(0);   // SSPM: Synchronous Serial Port Mode Select bits
             //   15 = I2C Slave mode, 10-bit address with Start and Stop bit
             //       interrupts enabled
             //   14 = I2 C Slave mode, 7-bit address with Start and Stop bit
             //       interrupts enabled
             //   11 = I2 C Firmware Controlled Master mode (Slave Idle)
             //   8 = I2C Master mode, clock = FOSC/(4 * (SSPADD + 1))
             //   7 = I2C Slave mode, 10-bit address
             //   6 = I2 C Slave mode, 7-bit address



   // SSPCON2: MSSP CONTROL REGISTER 2 (I2C MODE) (ADDRESS 91h)
   SSPCON2 = (unsigned char)
     +(0<<7) // GCEN: General Call Enable bit (Slave mode only)
             //   1 = Enable interrupt when a general call address (0000h) is
             //       received in the SSPSR
             //   0 = General call address disabled
     +(0<<6) // ACKSTAT: Acknowledge Status bit (Master Transmit mode only)
             //   1 = Acknowledge was not received from slave
             //   0 = Acknowledge was received from slave
     +(0<<5) // ACKDT: Acknowledge Data bit (Master Receive mode only)
             //   1 = Not Acknowledge
             //   0 = Acknowledge
     +(0<<4) // ACKEN: Acknowledge Sequence Enable bit (Master Receive mode only)
             //   1 = Initiate Acknowledge sequence on SDA and SCL pins and
             //       transmit ACKDT data bit. Automatically cleared by
             //       hardware.
             //   0 = Acknowledge sequence Idle
     +(0<<3) // RCEN: Receive Enable bit (Master mode only)
             //   1 = Enables Receive mode for I2
             //   C
             //   0 = Receive Idle
     +(0<<2) // PEN: Stop Condition Enable bit (Master mode only)
             //   1 = Initiate Stop condition on SDA and SCL pins.
             //       Automatically cleared by hardware.
             //   0 = Stop condition Idle
     +(0<<1) // RSEN: Repeated Start Condition Enabled bit (Master mode only)
             //   1 = Initiate Repeated Start condition on SDA and SCL pins.
             //       Automatically cleared by hardware.
             //   0 = Repeated Start condition Idle
     +(0);   // SEN: Start Condition Enabled/Stretch Enabled bit In Master mode:
             //   1 = Initiate Start condition on SDA and SCL pins.
             //       Automatically cleared by hardware.
             //   0 = Start condition Idle In Slave mode:
             //   1 = Clock stretching is enabled for both slave transmit and
             //       slave receive (stretch enabled)
             //   0 = Clock stretching is enabled for slave transmit only
             //       (PIC16F87X compatibility)



   // TXSTA: TRANSMIT STATUS AND CONTROL REGISTER (ADDRESS 98h)
   TXSTA = (unsigned char)
     +(0<<7) // CSRC: Clock Source Select bit Asynchronous mode: Don’t care.
             //       Synchronous mode:
             //   1 = Master mode (clock generated internally from BRG)
             //   0 = Slave mode (clock from external source)
     +(0<<6) // TX9: 9-bit Transmit Enable bit
             //   1 = Selects 9-bit transmission
             //   0 = Selects 8-bit transmission
     +(0<<5) // TXEN: Transmit Enable bit
             //   1 = Transmit enabled
             //   0 = Transmit disabled
     +(0<<4) // SYNC: USART Mode Select bit
             //   1 = Synchronous mode
             //   0 = Asynchronous mode
     +(0<<3) // Unimplemented: Read as ‘0’
     +(0<<2) // BRGH: High Baud Rate Select bit Asynchronous mode:
             //   1 = High speed
             //   0 = Low speed Synchronous mode: Unused in this mode.
     +(0<<1) // TRMT: Transmit Shift Register Status bit
             //   1 = TSR empty
             //   0 = TSR full
     +(0);   // TX9D: 9th bit of Transmit Data, can be Parity bit



   // RCSTA: RECEIVE STATUS AND CONTROL REGISTER (ADDRESS 18h)
   RCSTA = (unsigned char)
     +(0<<7) // SPEN: Serial Port Enable bit
             //   1 = Serial port enabled (configures RC7/RX/DT and RC6/TX/CK
             //       pins as serial port pins)
             //   0 = Serial port disabled
     +(0<<6) // RX9: 9-bit Receive Enable bit
             //   1 = Selects 9-bit reception
             //   0 = Selects 8-bit reception
     +(0<<5) // SREN: Single Receive Enable bit Asynchronous mode: Don’t
             //       care. Synchronous mode – Master:
             //   1 = Enables single receive
             //   0 = Disables single receive This bit is cleared after
             //       reception is complete. Synchronous mode – Slave:
             //       Don’t care.
     +(0<<4) // CREN: Continuous Receive Enable bit Asynchronous mode:
             //   1 = Enables continuous receive
             //   0 = Disables continuous receive Synchronous mode:
             //   1 = Enables continuous receive until enable bit CREN is
             //       cleared (CREN overrides SREN)
             //   0 = Disables continuous receive
     +(0<<3) // ADDEN: Address Detect Enable bit Asynchronous mode 9-bit (RX9 =
             //       1):
             //   1 = Enables address detection, enables interrupt and load of
             //       the receive buffer when RSR<8> is set
             //   0 = Disables address detection, all bytes are received and
             //       ninth bit can be used as parity bit
     +(0<<2) // FERR: Framing Error bit
             //   1 = Framing error (can be updated by reading RCREG register
             //       and receive next valid byte)
             //   0 = No framing error
     +(0<<1) // OERR: Overrun Error bit
             //   1 = Overrun error (can be cleared by clearing bit CREN)
             //   0 = No overrun error
     +(0);   // RX9D: 9th bit of Received Data (can be parity bit but must be calculated by user firmware)



   // ADCON0: REGISTER (ADDRESS 1Fh)
   ADCON0 = (unsigned char)
     +(0<<6) // ADCS: A/D Conversion Clock Select bits (ADCON0 bits in bold)
     +(0<<3) // CHS: Analog Channel Select bits
             //   0 = Channel 0 (AN0)
             //   1 = Channel 1 (AN1)
             //   2 = Channel 2 (AN2)
             //   3 = Channel 3 (AN3)
             //   4 = Channel 4 (AN4)
             //   5 = Channel 5 (AN5)
             //   6 = Channel 6 (AN6)
             //   7 = Channel 7 (AN7)
     +(0<<2) // GO/DONE: A/D Conversion Status bit When ADON = 1:
             //   1 = A/D conversion in progress (setting this bit starts the
             //       A/D conversion which is automatically cleared by hardware
             //       when the A/D conversion is complete)
             //   0 = A/D conversion not in progress
     +(0<<1) // Unimplemented: Read as ‘0’
     +(0);   // ADON: A/D On bit
             //   1 = A/D converter module is powered up
             //   0 = A/D converter module is shut-off and consumes no
             //       operating current



   // ADCON1: REGISTER (ADDRESS 9Fh)
   ADCON1 = (unsigned char)
     +(0<<7) // ADFM: A/D Result Format Select bit
             //   1 = Right justified. Six (6) Most Significant bits of ADRESH
             //       are read as ‘0’.
             //   0 = Left justified. Six (6) Least Significant bits of ADRESL
             //       are read as ‘0’.
     +(0<<6) // ADCS2: A/D Conversion Clock Select bit (ADCON1 bits in shaded area and in bold)
     +(0<<4) // Unimplemented: Read as ‘0’
     +(0);   // PCFG: A/D Port Configuration Control bits



   // CMCON: REGISTER
   CMCON = (unsigned char)
     +(0<<7) // C2OUT: Comparator 2 Output bit When C2INV = 0:
             //   1 = C2 VIN+ > C2 VIN-
             //   0 = C2 VIN+ < C2 VIN- When C2INV = 1:
             //   1 = C2 VIN+ < C2 VIN-
             //   0 = C2 VIN+ > C2 VIN-
     +(0<<6) // C1OUT: Comparator 1 Output bit When C1INV = 0:
             //   1 = C1 VIN+ > C1 VIN-
             //   0 = C1 VIN+ < C1 VIN- When C1INV = 1:
             //   1 = C1 VIN+ < C1 VIN-
             //   0 = C1 VIN+ > C1 VIN-
     +(0<<5) // C2INV: Comparator 2 Output Inversion bit
             //   1 = C2 output inverted
             //   0 = C2 output not inverted
     +(0<<4) // C1INV: Comparator 1 Output Inversion bit
             //   1 = C1 output inverted
             //   0 = C1 output not inverted
     +(0<<3) // CIS: Comparator Input Switch bit When CM2:CM0 = 110:
             //   1 = C1 VIN- connects to RA3/AN3 C2 VIN- connects to RA2/AN2
             //   0 = C1 VIN- connects to RA0/AN0 C2 VIN- connects to RA1/AN1
     +(0<<2) // CM: Comparator Mode bits Figure 12-1 shows the Comparator modes
             //       and CM2:CM0 bit settings.



   // CVRCON: CONTROL REGISTER (ADDRESS 9Dh)
   CVRCON = (unsigned char)
     +(0<<7) // CVREN: Comparator Voltage Reference Enable bit
             //   1 = CVREF circuit powered on
             //   0 = CVREF circuit powered down
     +(0<<6) // CVROE: Comparator VREF Output Enable bit
             //   1 = CVREF voltage level is output on RA2/AN2/VREF-/CVREF pin
             //   0 = CVREF voltage level is disconnected from
             //       RA2/AN2/VREF-/CVREF pin
     +(0<<5) // CVRR: Comparator VREF Range Selection bit
             //   1 = 0 to 0.75 CVRSRC, with CVRSRC/24 step size
             //   0 = 0.25 CVRSRC to 0.75 CVRSRC, with CVRSRC/32 step size
     +(0<<4) // Unimplemented: Read as ‘0’
     +(0);   // CVR: Comparator VREF Value Selection bits 0  VR3:VR0  15
             //       When CVRR = 1: CVREF = (VR<3:0>/ 24)  (CVRSRC) When
             //       CVRR = 0: CVREF = 1/4  (CVRSRC) + (VR3:VR0/ 32) 
             //       (CVRSRC)
     +(0<<13)// 13 bit0
     +(0<<13)// CP: Flash Program Memory Code Protection bit
             //   1 = Code protection off
             //   0 = All program memory code-protected
     +(0<<12)// Unimplemented: Read as ‘1’
     +(0<<11)// DEBUG: In-Circuit Debugger Mode bit
             //   1 = In-Circuit Debugger disabled, RB6 and RB7 are general
             //       purpose I/O pins
             //   0 = In-Circuit Debugger enabled, RB6 and RB7 are dedicated to
             //       the debugger
     +(0<<9) // WRT1:WRT0 Flash Program Memory Write Enable bits For
             //       PIC16F876A/877A:
             //   3 = Write protection off; all program memory may be written
             //       to by EECON control
             //   2 = 0000h to 00FFh write-protected; 0100h to 1FFFh may be
             //       written to by EECON control
             //   1 = 0000h to 07FFh write-protected; 0800h to 1FFFh may be
             //       written to by EECON control
             //   0 = 0000h to 0FFFh write-protected; 1000h to 1FFFh may be
             //       written to by EECON control For PIC16F873A/874A:
             //   3 = Write protection off; all program memory may be written
             //       to by EECON control
             //   2 = 0000h to 00FFh write-protected; 0100h to 0FFFh may be
             //       written to by EECON control
             //   1 = 0000h to 03FFh write-protected; 0400h to 0FFFh may be
             //       written to by EECON control
             //   0 = 0000h to 07FFh write-protected; 0800h to 0FFFh may be
             //       written to by EECON control
     +(0<<8) // CPD: Data EEPROM Memory Code Protection bit
             //   1 = Data EEPROM code protection off
             //   0 = Data EEPROM code-protected
     +(0<<7) // LVP: Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit
             //   1 = RB3/PGM pin has PGM function; low-voltage programming
             //       enabled
             //   0 = RB3 is digital I/O, HV on MCLR must be used for
             //       programming
     +(0<<6) // BOREN: Brown-out Reset Enable bit
             //   1 = BOR enabled
             //   0 = BOR disabled
     +(0<<4) // Unimplemented: Read as ‘1’
     +(0<<3) // PWRTEN: Power-up Timer Enable bit
             //   1 = PWRT disabled
             //   0 = PWRT enabled
     +(0<<2) // WDTEN: Watchdog Timer Enable bit
             //   1 = WDT enabled
             //   0 = WDT disabled
     +(0);   // FOSC: Oscillator Selection bits
             //   3 = RC oscillator
             //   2 = HS oscillator
             //   1 = XT oscillator
             //   0 = LP oscillator
#endif