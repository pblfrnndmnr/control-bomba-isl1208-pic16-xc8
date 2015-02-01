//CÛdigo original bajado de: http://www.todopic.com.ar/foros/index.php?topic=38654.0
//Gracias AngelGris
/**************************************************************************
*      Libreria para protocolo I2C Master por HardWare para HiTech        *
***************************************************************************

***************************************************************************
*                                                                         *
* Antes de incluir este archivo en nuestro programa hay que definir       *
* algunos parametros. Estos son                                           *
*                                                                         *
* PIC_CLK        (se usa para calcular el valor de SSPADD)                *     
* I2C_BAUD       (velocidad de transferencia)                             *
*                                                                         *
***************************************************************************

***************************************************************************
*                                                                         *
* En esta libreria se encuentran las siguientes funciones                 *
*                                                                         *
* setup_i2c(char)               (inicializa el puerto I2C)                *
* start_i2c()                   (envia start, inicia transmision)         *
* rstar_i2c()                   (envia restart, reinicia transmision)     *
* stop_i2c()                    (envia stop, termina transmision)         *
* write_i2c(char)               (envia un byte)                           *
* read_i2c(char)                (recibe un byte y envia ack o noack)      *
*                                                                         *
*  La funcion write_i2c() devuelve 0 si el dispositivo responde o 1 si el *
* dispositivo no responde.                                                *
*                                                                         *
*  A la funcion read_i2c() hay que pasarle 0 si se quiere responder al    *
* disipositivo o 1 si no se quiere responder                              *
*                                                                         *
*  En esta libreria estan definidos los parametros I2C_MASTER, I2C_SLAVE, *
* I2C_10BIT_ADDRESS. Eston son utilizados en la funcion setup_i2c(char)   *
*                                                                         *
*  Tambien estan definidos ACK y NOACK que son los utilizados para        *
* responder o no al dispositivo                                           *
*                                                                         *
***************************************************************************

* Ejemplo de configuracion y utilizacion

 #define PIC_CLK   10000000   // cristal 10MHz
 #define I2C_BAUD  100000     // velocidad 100 KHz
 #include "HardI2c.c"

 void main (void);
 {
   unsigned char respuesta;
   unsigned char dato;  
 
   ......
   ......
   ......
   setup_i2c (I2C_MASTER);
   start_i2c();
   respuesta = write_i2c (0b10101110);    // comunicacion para escribir en una 24C02C
   if (respuesta == ACK)
   {
     write_i2c (0x00);                    // indico la direccion a escribir
     write_i2c (0xF5);                    // escribo el dato F5
     write_i2c (0x06);                    // escribo el dato 06
   }
   stop_i2c();

   // colocar un delay necesario para que termine de grabar la memoria

   start_i2c();
   respuesta = write_i2c (0b10101110);    // comunicacion para escribir en una 24C02C
   if (respuesta == ACK)
   {
     write_i2c (0x00);                    // indico la direccion de memoria
     rstart_i2c();                       // reinicio la transmisi√≥n
     respuesta = write_i2c (0b10101111);  // comunicacion para leer en una 24C02C
     if (respuesta == ACK)
     {
       dato = read_i2c (ACK);             // leo el dato y respondo
       dato = read_i2c (NOACK);           // leo el dato y no respondo
     }
   }
   stop_i2c();
 }

*/
#include <xc.h>
#include <stdbool.h>       /* For true/false definition */
#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "HardI2C.h"


#if defined (PIC_CLK) && defined (I2C_BAUD)

void setup_i2c (char x)
 {
    TRISC3 = 1;

    TRISC4 = 1;

    SSPSTAT = 0;
    SSPCON = x;
    
    SSPCON2 = 0;
    // SSPCON2bits.RCEN=1;
    if (SSPM3 == 1) SSPADD = (unsigned char)((PIC_CLK/(4*I2C_BAUD)) - 1);

}

  char start_i2c (void)
  { 
    while (( SSPCON2 & 0x1F ) || R_nW );
    SEN = 1;
    return !BCLIF;
  }

  void rstart_i2c (void)
  {
    while (( SSPCON2 & 0x1F ) || R_nW );
    RSEN = 1;
  }

  void stop_i2c (void)
  {
    while (( SSPCON2 & 0x1F ) || R_nW );
    PEN = 1;
  }

  char write_i2c (unsigned char data)
  {
    if (SSPM3 == 1)
    {
      while (( SSPCON2 & 0x1F ) || R_nW );
      SSPBUF = data;
      while (R_nW == 1);
      //while(ACKSTAT==1);
      return ACKSTAT;
    }
    else
    {
      SSPBUF = data;
      CKP = 1;
      return 0;
    }
  }

  unsigned char read_i2c (unsigned char respuesta)
  {
    unsigned char dato;

    if (SSPM3 == 1)
    {
      while (( SSPCON2 & 0x1F ) || R_nW );
      RCEN = 1;
      while (( SSPCON2 & 0x1F ) || R_nW );
      dato = SSPBUF;
      while (( SSPCON2 & 0x1F ) || R_nW );
      ACKDT = respuesta;
      ACKEN = 1;
      return dato;
    }
    else
    {
      dato = SSPBUF;
      return dato;
    }
  }
#else
  #error Faltan definir parametros
#endif
