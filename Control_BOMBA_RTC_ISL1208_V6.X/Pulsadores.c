//Programa de gesti�n de pulsadores por Nocturno adaptado a xc8 por Pablo F. Manieri
//C�digo original bajado de: http://www.micropic.es/mpblog/2014/05/gestion-de-pulsadores/
// Gracias Nocturno
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "system.h"
#include "user.h"
#include "Pulsadores.h"

/****** HARDWARE ********
BOTON_Subir       	RB2
BOTON_Bajar       	RB1
BOTON_MENU       	RB3
BOTON_MANAUT     	RB4
BOTON_ONOFF		RB5
**************************/

bool Pulsacion(unsigned char Indice, unsigned char Pin, unsigned char TipoPin, bool Logica) {
   /*
      Esta funci�n gestiona las entradas de pulsador, mediante el control de un retardo
      antirrebotes y con la opci�n de autorrepetici�n de hasta 4 velocidades
      
      Para que funcione, hay que dimensionar la variable Entradas con el n� de pulsadores
      que queramos controlar.
      
      ENTRADAS
         Indice: hace referencia al n� de pulsador a chequear
         Pin: indica el estado del pin del pulsador (l�gica inversa)
         TipoPIN:
            TipoPin=0   -> No tiene autorrepetici�n
            TipoPin=1   -> Pin con autorrepetici�n
         
      SALIDA
         La funci�n devuelve un 1 si el pulsador est� en condiciones de ser procesado
         y un 0 en caso contrario.
      
   */
   static unsigned int Entradas[5]={0,0,0,0,0};  // Dimensionar en funci�n del n� de pulsadores
   unsigned int j;
  bool resultado=0;
   
   j=Entradas[Indice];  // Como leemos muchas veces el array, lo pasamos a variable local para 
                        // incrementar la velocidad de ejecuci�n
                        
   // Si se detecta pulsaci�n y antes no hab�a, devolvemos un 1 en resultado
   if ((((!Pin && Logica==LOGICA_INVERSA) || (Pin && Logica==LOGICA_DIRECTA)) && (j==0))) {
         resultado=1;
         j=1;
   };
   
   // Cada vez que se entra a la funci�n se incrementa una unidad el array
   if (j>0) 
         Entradas[Indice]++;
   
   // Si se detecta que ya no se est� pulsando, inicializamos a 0 el array
   if (((Pin && Logica==LOGICA_INVERSA) || (!Pin && Logica==LOGICA_DIRECTA)) && j>ANTIRREBOTES)
      Entradas[Indice]=0;
      
   if (TipoPin) { // Si el pulsador tiene autorrepetici�n
     // Cuando se supera el n� de ciclos 4, devolveremos un 1 con la frecuencia VELOCIDAD4
       if ((j>=CAMBIO4) && !(j%VELOCIDAD4)){
           resultado=1;
      }else
       // Cuando se supera el n� de ciclos 3, devolveremos un 1 con la frecuencia VELOCIDAD3
      if ((j>=CAMBIO3) && !(j%VELOCIDAD3)){           
         resultado=1;
      } else 
         // Cuando se supera el n� de ciclos 2, devolveremos un 1 con la frecuencia VELOCIDAD2
         if ((j>=CAMBIO2) && !(j%VELOCIDAD2)){
            resultado=1;
      } else
         // Cuando se supera el n� de ciclos 1, devolveremos un 1 con la frecuencia VELOCIDAD1
         if ((j>CAMBIO1) && !(j%VELOCIDAD1)){
            resultado=1;
      };
   }
   if ((TipoPin==INMEDIATO)&&(j>=1))
       resultado=1;
   return (resultado);
}


   

