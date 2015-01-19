#ifndef PULSADORES_H
#define PULSADORES_H
/****** HARDWARE ********

 Teclas usadas
SUBIR-      RB1
BAJAR-      RB2
MENU-       RB3
MANAUT-    RB4
ONOFF-     RB5

 **************************/

//TODO cambiado antirrebotes de 20 a 50 para probar mejor respuesta de los pulsadores
#define ANTIRREBOTES    50    // Nº de ciclos mínimo antes de detectar otra pulsación
#define CAMBIO1         100   // Nº de ciclos para el primer intervalo
#define CAMBIO2         500   // Nº de ciclos para el segundo intervalo
#define CAMBIO3         2000  // Nº de ciclos para el tercer intervalo
#define CAMBIO4         4000  // Nº de ciclos para el tercer intervalo
#define VELOCIDAD1      100   // Velocidad de autorrepetición más lenta
#define VELOCIDAD2      50    // Velocidad de autorrepetición mediana
#define VELOCIDAD3      10    // Velocidad de autorrepetición más rápida
#define VELOCIDAD4      2    // Velocidad de autorrepetición más rápida
#define SIN_REPETICION  0     // La tecla no repite si se mantiene pulsada
#define CON_REPETICION  1     // La tecla sí repite si se mantiene pulsada
#define INMEDIATO       2     // La tecla se mantiene pulsada, da siempre un uno
#define LOGICA_DIRECTA  0     // Al pulsar se recibe un 0
#define LOGICA_INVERSA  1     // Al pulsar se recibe un 1


#define BOTON_Subir      PORTBbits.RB1
#define BOTON_Bajar      PORTBbits.RB2
#define BOTON_MENU       PORTBbits.RB3
#define BOTON_MANAUT     PORTBbits.RB4
#define BOTON_ONOFF      PORTBbits.RB5




bool Pulsacion(unsigned char Indice, unsigned char Pin, unsigned char TipoPin, bool Logica);
//void RefrescaDisplay();
//void Digitos(unsigned int Valor);
//void interrupcion_pulsadores(void);


#define testbit(var, bit) ((var) && (1 <<(bit)))  //retorna 1 si es verdadero, 0 si es falso
#define setbit(var, bit) ((var) |= (1 << (bit)))  //pone en 1 el bit correspondiente a bit de la variable var
#define clrbit(var, bit) ((var) &= ~(1 << (bit))) //pone en 0 el bit correspondiente a bit de la variable var




#endif