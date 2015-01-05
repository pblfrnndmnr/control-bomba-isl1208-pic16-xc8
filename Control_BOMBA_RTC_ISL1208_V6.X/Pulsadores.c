//Programa de gestión de pulsadores por Nocturno adaptado a xc8 por Pablo F. Manieri

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "system.h"
#include "user.h"
#include "Pulsadores.h"

/****** HARDWARE ********
BOTON_Subir       	RB1
BOTON_Bajar       	RB2
BOTON_MENU       	RB3
BOTON_MANAUT     	RB4
BOTON_ONOFF		RB5
**************************/

bool Pulsacion(unsigned char Indice, unsigned char Pin, unsigned char TipoPin, bool Logica) {
   /*
      Esta función gestiona las entradas de pulsador, mediante el control de un retardo
      antirrebotes y con la opción de autorrepetición de hasta 4 velocidades
      
      Para que funcione, hay que dimensionar la variable Entradas con el nº de pulsadores
      que queramos controlar.
      
      ENTRADAS
         Indice: hace referencia al nº de pulsador a chequear
         Pin: indica el estado del pin del pulsador (lógica inversa)
         TipoPIN:
            TipoPin=0   -> No tiene autorrepetición
            TipoPin=1   -> Pin con autorrepetición
         
      SALIDA
         La función devuelve un 1 si el pulsador está en condiciones de ser procesado
         y un 0 en caso contrario.
      
   */
   static unsigned int Entradas[5]={0,0,0,0,0};  // Dimensionar en función del nº de pulsadores
   unsigned int j;
  bool resultado=0;
   
   j=Entradas[Indice];  // Como leemos muchas veces el array, lo pasamos a variable local para 
                        // incrementar la velocidad de ejecución
                        
   // Si se detecta pulsación y antes no había, devolvemos un 1 en resultado
   if ((((!Pin && Logica==LOGICA_INVERSA) || (Pin && Logica==LOGICA_DIRECTA)) && (j==0))) {
         resultado=1;
         j=1;
   };
   
   // Cada vez que se entra a la función se incrementa una unidad el array
   if (j>0) 
         Entradas[Indice]++;
   
   // Si se detecta que ya no se está pulsando, inicializamos a 0 el array
   if (((Pin && Logica==LOGICA_INVERSA) || (!Pin && Logica==LOGICA_DIRECTA)) && j>ANTIRREBOTES)
      Entradas[Indice]=0;
      
   if (TipoPin) { // Si el pulsador tiene autorrepetición
     // Cuando se supera el nº de ciclos 4, devolveremos un 1 con la frecuencia VELOCIDAD4
       if ((j>=CAMBIO4) && !(j%VELOCIDAD4)){
           resultado=1;
      }else
       // Cuando se supera el nº de ciclos 3, devolveremos un 1 con la frecuencia VELOCIDAD3
      if ((j>=CAMBIO3) && !(j%VELOCIDAD3)){           
         resultado=1;
      } else 
         // Cuando se supera el nº de ciclos 2, devolveremos un 1 con la frecuencia VELOCIDAD2
         if ((j>=CAMBIO2) && !(j%VELOCIDAD2)){
            resultado=1;
      } else
         // Cuando se supera el nº de ciclos 1, devolveremos un 1 con la frecuencia VELOCIDAD1
         if ((j>CAMBIO1) && !(j%VELOCIDAD1)){
            resultado=1;
      };
   }
   if ((TipoPin==INMEDIATO)&&(j>=1))
       resultado=1;
   return (resultado);
}


   

