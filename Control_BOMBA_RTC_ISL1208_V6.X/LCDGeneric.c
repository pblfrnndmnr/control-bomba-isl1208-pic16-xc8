/**
   \version: 1.0

   
   \author Suky (Casanova Alejandro) 
   \web www.infopic.comlu.com
   \date 16/08/10


///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                                                                   ////
////        (C) Copyright 2010 www.infopic.comlu.com                   ////
//// Este código puede ser usado, modificado y distribuido libremente  ////
//// sin eliminar esta cabecera y  sin garantía de ningún tipo.        ////
////                                                                   ////
////                                                                   ////
/////////////////////////////////////////////////////////////////////////// 
   
 *- Version Log --------------------------------------------------------------*
 *   Fecha       Autor                Comentarios                             *
 *----------------------------------------------------------------------------*
 * 16/08/10      Suky               Original (Rev 1.0)                        *
 * 20/08/10      Suky               Se incluye CodeVisionAVR                  *
 *----------------------------------------------------------------------------* */
#include "xc.h"
#include "system.h"
#include "LCDGeneric.h"

UINT8 NLinea;

UINT8 ui8BusyLCD(void){

#ifdef LCD_USE_RW
	 LCD_DATA_PIN_7 = 0;
	 LCD_DATA_PIN_6 = 0;
	 LCD_DATA_PIN_5 = 0;
	 LCD_DATA_PIN_4 = 0;

     LCD_TRIS_DATA_PIN_7 = CONFIG_INPUT;
     LCD_TRIS_DATA_PIN_6 = CONFIG_INPUT;
     LCD_TRIS_DATA_PIN_5 = CONFIG_INPUT;
     LCD_TRIS_DATA_PIN_4 = CONFIG_INPUT;

     LCD_RW_PIN = 1;    
     LCD_RS_PIN = 0; 
	 LCD_E_PIN=1;         
     __delay_1us();

     if(LCD_READ_PIN_7==1){
          LCD_E_PIN = 0; 
		  __delay_1us();
		  LCD_E_PIN = 1;
		  __delay_1us();
		  LCD_E_PIN = 0;
      	  LCD_RW_PIN = 0; 
          return 1;
     }else{
          LCD_E_PIN = 0;
		  __delay_1us();
		  LCD_E_PIN = 1;
		  __delay_1us();
		  LCD_E_PIN = 0;
		  LCD_RW_PIN = 0;       
          return 0;
     }
#else
	 __delay_100us();
	 return 0;
#endif
}

void vWriteLCD(UINT8 Data,UINT8 Type){

#ifdef LCD_USE_3PINES
	UINT8 i, data_temp;
#endif

	while(ui8BusyLCD());

	#ifndef LCD_USE_3PINES
		#ifdef LCD_USE_RW
			LCD_RW_PIN = 0;
		#endif   
		if(Type){
			LCD_RS_PIN=1;
		}else{
			LCD_RS_PIN = 0;
		}	  
		LCD_TRIS_DATA_PIN_7 = CONFIG_OUTPUT;
		LCD_TRIS_DATA_PIN_6 = CONFIG_OUTPUT;
		LCD_TRIS_DATA_PIN_5 = CONFIG_OUTPUT;
		LCD_TRIS_DATA_PIN_4 = CONFIG_OUTPUT;
		
		LCD_DATA_PIN_7 = !!(Data & 0x80);
		LCD_DATA_PIN_6 = !!(Data & 0x40);
		LCD_DATA_PIN_5 = !!(Data & 0x20);
		LCD_DATA_PIN_4 = !!(Data & 0x10);
	#else
		data_temp=(Data>>4);  // Rs es bit 4
		if(Type){
			data_temp|=0x10;
		}
		for(i=0;i<8;i++){
			LCD_DATA_PIN=!!(data_temp & 0x80);
			data_temp<<=1;
			LCD_CLOCK_PIN=1;
			__delay_1us();
			LCD_CLOCK_PIN=0;
		}
	#endif
	__delay_1Cycle();
	LCD_E_PIN = 1;
	__delay_1us();
	LCD_E_PIN = 0;
	#ifndef LCD_USE_3PINES	
		LCD_DATA_PIN_7 = !!(Data & 0x08);
		LCD_DATA_PIN_6 = !!(Data & 0x04);
		LCD_DATA_PIN_5 = !!(Data & 0x02);
		LCD_DATA_PIN_4 = !!(Data & 0x01);
	#else
		data_temp=(Data&0x0F);  // Rs es bit 4
		if(Type){
			data_temp|=0x10;
		}
		for(i=0;i<8;i++){
			LCD_DATA_PIN=!!(data_temp & 0x80);
			data_temp<<=1;
			LCD_CLOCK_PIN=1;
			__delay_1us();
			LCD_CLOCK_PIN=0;
		}
	#endif
	__delay_1Cycle();	
	LCD_E_PIN = 1;
	__delay_1us();
	LCD_E_PIN = 0;
	
} 

void vInitLCD(){
UINT8 i;
#ifdef LCD_USE_3PINES
	UINT8 temp;
#endif

	for(i=0;i<8;i++){__delay_2ms();}
	NLinea=1;
	
#ifndef LCD_USE_3PINES
/* ** Configuración de pines ** */	
	LCD_DATA_PIN_7 = 0;
	LCD_DATA_PIN_6 = 0;
	LCD_DATA_PIN_5 = 0;
	LCD_DATA_PIN_4 = 0;
	#ifdef LCD_USE_RW
		LCD_RW_PIN = 0;
	#endif
	LCD_RS_PIN = 0;
	LCD_E_PIN = 0;
	
	LCD_TRIS_DATA_PIN_7 = CONFIG_OUTPUT;
	LCD_TRIS_DATA_PIN_6 = CONFIG_OUTPUT;
	LCD_TRIS_DATA_PIN_5 = CONFIG_OUTPUT;
	LCD_TRIS_DATA_PIN_4 = CONFIG_OUTPUT;
	#ifdef LCD_USE_RW	
		LCD_TRIS_RW = CONFIG_OUTPUT;
	#endif
	LCD_TRIS_RS = CONFIG_OUTPUT;
	LCD_TRIS_E = CONFIG_OUTPUT;
#else
	LCD_DATA_PIN = 0;
	LCD_CLOCK_PIN = 0;
	LCD_E_PIN = 0;

	LCD_TRIS_DATA = CONFIG_OUTPUT;
	LCD_TRIS_CLOCK = CONFIG_OUTPUT;
	LCD_TRIS_E = CONFIG_OUTPUT;
	
	for(i=0;i<8;i++){
		LCD_DATA_PIN=0;
		LCD_CLOCK_PIN=1;
		__delay_1us();
		LCD_CLOCK_PIN=0;
	}
#endif
/* ** INICIALIZACION ** */     
#ifndef LCD_USE_3PINES
	LCD_DATA_PIN_5 = 1;
	LCD_DATA_PIN_4 = 1;
#else
	temp=0x03;
	for(i=0;i<8;i++){
		LCD_DATA_PIN=!!(temp & 0x80);
		temp<<=1;
		LCD_CLOCK_PIN=1;
		__delay_1us();
		LCD_CLOCK_PIN=0;
	}
#endif
	for(i=0;i<3;i++){
		LCD_E_PIN = 1;
		__delay_2ms();
		LCD_E_PIN = 0;
	 	__delay_2ms();
	}
#ifndef LCD_USE_3PINES
	LCD_DATA_PIN_4 = 0;
#else
	temp=0x02;
	for(i=0;i<8;i++){
		LCD_DATA_PIN=!!(temp & 0x80);
		temp<<=1;
		LCD_CLOCK_PIN=1;
		__delay_1us();
		LCD_CLOCK_PIN=0;
	}
#endif
	LCD_E_PIN = 1;
	__delay_1us();
	LCD_E_PIN = 0;

	vWriteLCD(0x20 | (LcdType<<2),LCD_COMMAND);  // Tipo display.-  
	__delay_2ms();     
	vWriteLCD(0x01,LCD_COMMAND);	// Borramos display.-   
	__delay_2ms();           
	vWriteLCD(0x06,LCD_COMMAND);	// Incrementa cursor.-
	vWriteLCD(0x0C,LCD_COMMAND);	// Encendemos display.-
}


void vLCD_Putc(UINT8 Data){
	
	switch(Data){
		case '\f':
			vWriteLCD(0x01,LCD_COMMAND);
			NLinea=1;
			__delay_2ms();
		break;
		case '\n':
			vGotoxyLCD(1,++NLinea);			
		break;
		default:
			vWriteLCD(Data,LCD_DATA);
	}
} 

void vGotoxyLCD(UINT8 x,UINT8 y){
UINT8 Direccion;

	switch(y){
		case 1:Direccion = LCD_LINE_1_ADDRESS;NLinea=1;break;
		case 2:Direccion = LCD_LINE_2_ADDRESS;NLinea=2;break;
		#ifdef LCD_4LINES
		case 3:Direccion = LCD_LINE_3_ADDRESS;NLinea=3;break;
		case 4:Direccion = LCD_LINE_4_ADDRESS;NLinea=4;break;
		#endif
		default:Direccion = LCD_LINE_1_ADDRESS;NLinea=1;break;
	}

	Direccion+=x-1;
	while(ui8BusyLCD());
	vWriteLCD(0x80|Direccion,LCD_COMMAND);
}

void vPuts_LCD(UINT8 *buffer){


    while(*buffer != '\0')    {
       // vLCD_Putc(*buffer++);;
        vWriteLCD(*buffer++,LCD_DATA);
    }
}

void vBlinkingLCD(void){
	vWriteLCD(0b1101,LCD_COMMAND);
}
void vnoBlinkingLCD(void){
	vWriteLCD(0b1100,LCD_COMMAND);
}