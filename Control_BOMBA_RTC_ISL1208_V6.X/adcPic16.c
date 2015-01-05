#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "system.h"
#include "user.h"
#include "adcPic16.h"

void setADCChannel(char canal) {
    ADCON0bits.CHS = canal; //Selecciono el canal AD0
};

char getADCChannel(void) {
    return ADCON0bits.CHS; //Selecciono el canal AD0
};

void openADC(void) {

    TRISA0 = 1; //configuro como entrada para el ADC0
    TRISA1 = 1; //configuro como entrada para el ADC1
    TRISA3 = 1; //Configuro como entrada para Vref+
    TRISA2 = 0; //No usado
    TRISA4 = 0;//No usado
    TRISA5 = 0;//No usado
    ADCON1bits.PCFG = 0b101; //An3 como VREF+ , AN0 y AN1 como entrada analogica, AN0 Corriente, AN1 Tension de red
    ADCON1bits.ADFM = 1; //justificado a la derecha
    ADCON1bits.ADCS2 = 1;
    ADCON0bits.ADCS = 0b10; //Tosc/64
    ADCON0bits.ADON = 1; //Activo el ADC

}

unsigned int adcRead(void) {

    return ((unsigned int) ADRESH << 8) | ADRESL;
}

void startADC(void) {
    ADCON0bits.GO_DONE = 1;
}

void interruptADC_on(void) {
    ADIE = 1;
}

void interruptADC_off(void) {
    ADIE = 0;
}