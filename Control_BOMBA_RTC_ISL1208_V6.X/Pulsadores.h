//Programa de gesti�n de pulsadores por Nocturno adaptado a xc8 por Pablo F. Manieri
//C�digo original bajado de: http://www.micropic.es/mpblog/2014/05/gestion-de-pulsadores/
// Gracias Nocturno
#ifndef PULSADORES_H
#define PULSADORES_H
/****** HARDWARE ********

 Teclas usadas
SUBIR-      RB2
BAJAR-      RB1
MENU-       RB3  //si se pulsa y se suelta menu usuario,TODO (ver como implementarlo) si se pulsa y se mantiene pulsada menu service
MANAUT-     RB4  //cambia entre manual y automatico, si est� en el menu inicial. Sino es tecla ESC (se sale de cualquier menu)
ONOFF-      RB5  //apaga o enciende la bomba en manual si esta en menu inicil. Es boton grabar si est� en los menu de seteo de datos.

 **************************/


#define ANTIRREBOTES    50    // N� de ciclos m�nimo antes de detectar otra pulsaci�n
#define CAMBIO1         100   // N� de ciclos para el primer intervalo
#define CAMBIO2         500   // N� de ciclos para el segundo intervalo
#define CAMBIO3         2000  // N� de ciclos para el tercer intervalo
#define CAMBIO4         4000  // N� de ciclos para el tercer intervalo
#define VELOCIDAD1      100   // Velocidad de autorrepetici�n m�s lenta
#define VELOCIDAD2      50    // Velocidad de autorrepetici�n mediana
#define VELOCIDAD3      10    // Velocidad de autorrepetici�n m�s r�pida
#define VELOCIDAD4      2    // Velocidad de autorrepetici�n m�s r�pida
#define SIN_REPETICION  0     // La tecla no repite si se mantiene pulsada
#define CON_REPETICION  1     // La tecla s� repite si se mantiene pulsada
#define INMEDIATO       2     // La tecla se mantiene pulsada, da siempre un uno
#define LOGICA_DIRECTA  0     // Al pulsar se recibe un 0
#define LOGICA_INVERSA  1     // Al pulsar se recibe un 1


#define BOTON_Subir      RB2 //PORTBbits.RB1  //Cambiado rb2 por rb1 para que qude mejor ubicado los botones bajar y subir
#define BOTON_Bajar      RB1 //PORTBbits.RB2
#define BOTON_MENU       RB3 //PORTBbits.RB3
#define BOTON_MANAUT     RB4 //PORTBbits.RB4
#define BOTON_ONOFF      RB5 //PORTBbits.RB5

bool Pulsacion(unsigned char Indice, unsigned char Pin, unsigned char TipoPin, bool Logica);

#define testbit(var, bit) ((var) && (1 <<(bit)))  //retorna 1 si es verdadero, 0 si es falso
#define setbit(var, bit) ((var) |= (1 << (bit)))  //pone en 1 el bit correspondiente a bit de la variable var
#define clrbit(var, bit) ((var) &= ~(1 << (bit))) //pone en 0 el bit correspondiente a bit de la variable var

#endif