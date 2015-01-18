#ifndef	_USER_H_
#define	_USER_H_
/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#define salidabomba PORTCbits.RC0


//contadores para minutos segundos y horas
unsigned char cuentasegundos = 0, cuentaminutos = 0, cuentahoras = 0, cuentadias = 0, cuentameses = 0;
unsigned char cuenta40ms;
unsigned char cuenta500ms;

#define TIEMPODEESPERA=10 //segundos
unsigned char tiempoanterior = 0, tiempoactual = 0;
bool expirotiempo = 0;
unsigned char activapor;
/*  Application specific user parameters used in user.c may go here */


#define LCD 0
#define USART 1
//bandera para enviar por USART o por LCD
bool envia_por = LCD;
bool refrescadisplay = 0;
bool haycambio = 0;
//Definiciones para indicar el inicio de la posicion de los datos del LCD
#define INICIOHORAS 1
#define INICIOMINUTOS INICIOHORAS+3
#define INICIODIA 1
#define INICIOMES INICIODIA+3
#define INICIOANIO INICIOMES+3
bool flanco = 0;

//constantes para saber que se esta modificando

#define HORA 0X00
#define MINUTOS 0X01
#define DIA 0X02
#define MES 0X03
#define ANIO 0X04
#define DIADELASEMANA 0X05
#define SINO 0x06
#define TIEMPOENCENDIDO 0x07
#define TIEMPOFALLA 0x08
#define PERIODOENCENDIDO 0X09
#define NOHACENADA 0x0A
//bandera para modificar minutos horas dia mes año
bool modificafecha = NOHACENADA;
bool banderasino = 0;

//definicion de los datos para la fecha y horario

typedef union {//32bits
    unsigned long Valor;

    struct {
        unsigned char day;
        unsigned char month;
        unsigned char yr;
        unsigned char dow;
    };
} Fecha_t;

typedef union {//24bits
    unsigned short long Valor;

    struct {
        unsigned char sec;
        unsigned char min;
        unsigned char hrs;

    };
} Horario_t;

//ISL1208 Status Register (SR) - Address 07h

typedef union {//8bits
    unsigned char Valor;

    struct {
       unsigned RTCF:1; //REAL TIME CLOCK FAIL BIT (RTCF)
        unsigned BAT:1; //BATTERY BIT (BAT)
        unsigned ALM:1; //ALARM BIT (ALM)
        unsigned : 1;
        unsigned WRTC:1; //WRITE RTC ENABLE BIT (WRTC)
        unsigned : 1;
        unsigned XTOSCB:1; //CRYSTAL OSCILLATOR ENABLE BIT (XTOSCB)
       unsigned ARST:1; //AUTO RESET ENABLE BIT (ARST)

    };
} isl1208_SR_t;

//ISL1208 Interrupt Control Register (INT) - Address 08h

typedef union {//8bits
    unsigned char Valor;

    struct {
        unsigned FO : 4; //FREQUENCY OUT CONTROL BITS (FO <3:0>)
        unsigned FOBATB:1; //FREQUENCY OUTPUT AND INTERRUPT BIT (FOBATB)
        unsigned LPMODE:1; //LOW POWER MODE BIT (LPMODE)
        unsigned ALME:1; //ALARM ENABLE BIT (ALME)
        unsigned IM:1; //INTERRUPT/ALARM MODE BIT (IM)
    };
} isl1208_INT_t;



Fecha_t fecha;
Horario_t * horario;
Horario_t horarioenc;
Horario_t horarioactual;
isl1208_SR_t isl1208SR;
isl1208_INT_t isl1208INT;
//char sdow[11];

bool bandera_startglobal = 0;
//bool bandera_modificaglobal=0;
char cadenaamostrar[8];
char cadenaamostrar2[8];
bool bandera_grabafechay_hora = 0;
float mediciondecorriente;

//Definicion de las entradas del menu
#define MENU_MUESTRAHORA 0 //muestra la hora actual y el estado de la bomba, encendido apagado o falla de sobrecorriente, nivel bajo
#define MENU_MUESTRAFECHA 1 //muestra la fecha actual
#define MENU_MUESTRAMEDICIONES 2 //menu que muestra la medicion de corriente de motor y tension
#define MENU_CONFIGURAHORARIO 3 //menu para configurar el horario
//#este menu tiene submenu
#define SUBMENU_CONFIGURAHORA 4 //submenu para configurar la hora
#define SUBMENU_CONFIGURAMINUTOS 5 //submenu para configurar los minutos

#define MENU_CONFIGURAFECHA 6 //menu para configurar la fecha
//#este menu tiene submenu
#define SUBMENU_CONFIGURADIA 7 //submenu para configurar el dia
#define SUBMENU_CONFIGURAMES 8 //submenu para configurar el mes
#define SUBMENU_CONFIGURAANIO 9 //submenu para configurar el año


#define MENU_CONFIGURAENCENDIDO 10 //menu para configurar la hora de encendido de la bomba
//#este menu tiene submenu
#define SUBMENU_CONFIGURAHORAENCENDIDO 11 //submenu para configurar la hora de encendido
#define SUBMENU_CONFIGURAMINUTOSENCENDIDO 12 //submenu para configurar la hora de encendido
#define SUBMENU_CONFIGURATIEMPOENCENDIDO 13 //submenu para configurar el tiempo en que la bomba está encendida, con un maximo de 1hora
#define SUBMENU_CONFIGURAPERIODOENCENDIDO 14 //submenu para indicar si se repite el encendido cada 12 horas

#define MENU_CONFIGURAFALLACORRIENTE 15//se indica si la entrada de falla de corriente es por contacto NC=1 o NA=0
#define MENU_CONFIGURATIEMPOFALLACORRIENTE 16 //se configura el tiempo en que la falla esta activa para parar el motor, maximo 30segundos
#define MENU_CONFIGURAINDICACIONDENIVEL 17  //se indica si la entrada de deteccion de nivel es por contacto NC=1 o NA=0

#define ULTIMOMENU 17 //indica el valor del ultimo menu

#define TIEMPOMAXIMOENCENDIDO 60 //minutos
#define TIEMPOMAXIMOFALLA 30 //segundos 
#define TIEMPOMAXIMONIVEL 15 //es el tiempo que hay que esperar en caso de que se llegue al nivel minimo para apagar el motor
#define TIEMPOMAXIMOPERIODO 7 //cada cuantos dias se riega.
unsigned char periodoencendido = 1;
unsigned char tiempoencendido = 0;
unsigned char tiempofalla = 0;
unsigned char menuactual = MENU_MUESTRAHORA;

//constantes y variables de indicacion del estado de la bomba
#define APAGABOMBA 0
#define ENCIENDEBOMBA 1
bool activabomba = 0;

#define BOMBAAPAGADA 0
#define BOMBAENCENDIDA 1
bool estadobomba = 0;
#define NIVELNORMAL 0
#define NIVELBAJO 1
bool estadonivel = 0;
#define CORRIENTENORMAL 0
#define FALLACORRIENTE 1
bool estadofallacorriente = 0;
#define VOLTAJENORMAL 0
#define FALLAVOLTAJE 1
bool estadofallavoltaje= 0;
unsigned int medidaI_adc = 0;
#define OFFSET_I .3
unsigned int medidaV_adc = 0;
bool cambio_de_canal = 0;
int promedio_adc[4] = {0, 0, 0, 0};
/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

/* User level functions prototypes (i.e. InitApp) go here */
void InitApp(void); /* I/O and Peripheral Initialization */
void putch(char data);

#endif
