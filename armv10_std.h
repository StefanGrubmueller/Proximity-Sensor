/******************************************************************************/
/*   (C) Copyright HTL - HOLLABRUNN  2009-2010 All rights reserved. AUSTRIA   */ 
/*                                                                            */ 
/* File Name:   armv10_std.h                                                  */
/* Autor: 		Josef Reisinger                                               */
/* Version: 	V1.00                                                         */
/* Date: 		07/05/2010                                                    */
/* Description: Standard Library für ARM Corttex M3                           */
/******************************************************************************/
/* History: V1.00  creation										              */
/* 			V1.01  09.06.2010 REJ:                                            */
/*                 Add Bit banding Adresses for LED/Switsches Board           */
/*          V2.0   19.02.2011 REJ:                                            */
/*                 - dil_taster_init, dil_led_init hinzugefügt                */
/*                 - lcd_setcursor auf 4 zelige anzeige erweiterst            */
/*                 - Bitbanding Definitiones von Pointer (Adresse der 		  */
/*                   Speicherzelle auf Inhalt der Speicherzelle umgestellt    */
/*				   - Fehler bei Portmodi Settings in Init_led Switsches 	  */ 
/*                   korrigiert												  */
/******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ARMV10_STD_H
#define __ARMV10_STD_H

/* Includes ------------------------------------------------------------------*/
#include <stm32f10x.h>
#include <stdio.h>

/*----------------------------- Define Pins for LED and Swistches--------------*/
/* Einzel-Portpins im Bitbandbereich festlegen                                 */

#define GPIOA_IDR GPIOA_BASE + 2*sizeof(uint32_t)    // Calc peripheral address GPIOA IDR
#define GPIOA_ODR GPIOA_BASE + 3*sizeof(uint32_t)
#define GPIOB_IDR GPIOB_BASE + 2*sizeof(uint32_t)
#define GPIOB_ODR GPIOB_BASE + 3*sizeof(uint32_t)	 // Calc peripheral address GPIOB ODR
#define GPIOC_IDR GPIOC_BASE + 2*sizeof(uint32_t)
#define GPIOC_ODR GPIOC_BASE + 3*sizeof(uint32_t)

// Calc Bit Band Adress from peripheral address: a = peripheral address b = Bit number
#define BITBAND_PERI(a,b) ((PERIPH_BB_BASE + (a-PERIPH_BASE)*32 + (b*4)))  

#define SW0  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,0)))	// PA0
#define SW1  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,1)))	// PA1
#define SW2  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,2)))	// PA2
#define SW3  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,3)))	// PA3
#define SW4  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,4)))	// PA4
#define SW5  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,5)))	// PA5
#define SW6  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,6)))	// PA6
#define SW7  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,7)))	// PA7

#define PIEZO *((volatile unsigned long *)(BITBAND_PERI(GPIOB_ODR,0)))	//Portleitung des Piezo-Summers (PB0)

#define LED0  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_ODR,8)))	// PB8
#define LED1  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_ODR,9)))	// PB9
#define LED2  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_ODR,10)))	// PB10
#define LED3  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_ODR,11)))	// PB11
#define LED4  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_ODR,12)))	// PB12
#define LED5  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_ODR,13)))	// PB13
#define LED6  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_ODR,14)))	// PB14
#define LED7  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_ODR,15)))	// PB15

#define DIL_LED1  *((volatile unsigned long *)(BITBAND_PERI(GPIOD_ODR,2)))		//PD2
#define DIL_LED2  *((volatile unsigned long *)(BITBAND_PERI(GPIOC_ODR,13)))	//PC13
#define DIL_LED3  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_ODR,8)))		//PA8

#define DIL_TASTER1  *((volatile unsigned long *)(BITBAND_PERI(GPIOC_IDR,5)))	//PC5
#define DIL_TASTER2  *((volatile unsigned long *)(BITBAND_PERI(GPIOC_IDR,13)))	//PC13 
#define DIL_TASTER3  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,8)))	//PA8



/*--------------------------------- Definitions LCD --------------------------*/
#define LineLen     16                  /* Width of LCD (in characters)       */
#define NumLines     2                  /* Hight of LCD (in lines)            */

/* ---------------------Vordefinierte Frequenzen fuer Tonerzeugung -----------*/
#define ton_C1 262
#define ton_D1 293
#define ton_E1 329
#define ton_F1 349
#define ton_G1 391
#define ton_A1 440
#define ton_H1 493
#define ton_C2 523
#define ton_D2 587
#define ton_E2 659
#define ton_F2 698
#define ton_G2 783 
#define ton_A2 880
#define ton_H2 987
#define ton_C3 1046

/* ------------------- Vordefinierte Längen (ms) für Tonerzeugung --------------*/
#define ganze		800
#define halbe		400
#define viertel 	200
#define achtel		100
#define sechzehntel	50


#ifdef ARMV10_STD_MOD
#define EXPORT
#else
#define EXPORT extern
#endif 

/* ----------------------------Exported functions ---------------------------- */

/* ------------------------- LED/ Switches Board Funktions -------------------*/

/******************************************************************************/
/*            U N T E R P R O G R A M M:    init_leds_switches                */
/*                                                                            */
/* Aufgabe:   Initialisiert Portleitungen für LED / Schalterplatine           */
/* Input:                                                                     */
/* return:	                                                                  */
/******************************************************************************/
EXPORT void init_leds_switches(void); 

/*******************************************************************************/
/*            U N T E R P R O G R A M M:    set_leds                          */
/*                                                                            */
/* Aufgabe:  gibt hexadezimalen Wert in auf 8 Leds aus                        */
/*           (an Port 0 angeschlossen)                                        */
/* Input:    value = Wert auf den LEDS gesetzt werden sollen                  */                                                               
/* return:	                                                                  */
/******************************************************************************/
EXPORT void set_leds (char value);

/******************************************************************************/
/*            U N T E R P R O G R A M M:    get_switches                      */
/*                                                                            */
/* Aufgabe:   liest aktuelle Schalterstellung ein                             */
/* Input:                                                                     */
/* return:	  aktuelle Schalterstellung                                       */
/******************************************************************************/
EXPORT char get_switches(void); 

/******************************************************************************/
/*            U N T E R P R O G R A M M:    dil_taster_init                   */
/*                                                                            */
/* Aufgabe:   Initialisiert Portleitung für Taster auf DIL                    */
/* Input:                                                                     */
/* return:	                                                                  */
/******************************************************************************/
EXPORT void dil_taster_init(void);

/******************************************************************************/
/*            U N T E R P R O G R A M M:    dil_led _init                     */
/*                                                                            */
/* Aufgabe:   Initialisiert Portleitung für LEDS auf DIL                      */
/* Input:                                                                     */
/* return:	                                                                  */
/******************************************************************************/
EXPORT void dil_led_init(void);


/* ------------------------- Miscellaneous Funktions --------------------------*/


/******************************************************************************/
/*            U N T E R P R O G R A M M:    WAIT_MS                           */
/*                                                                            */
/* Aufgabe:   Wartet die angegebene Anzahl an Millisekunden                   */
/* Input:     ms = Anzahl der zu wartenden Millisekunden                      */
/* return:	 	                                                              */
/******************************************************************************/
EXPORT void wait_ms(int ms);

/******************************************************************************/
/*            U N T E R P R O G R A M M:    wait_10us                         */
/*                                                                            */
/* Aufgabe:   Wartet 10µs                                                     */
/* Input:                                                                     */
/* return:	 	                                                              */
/******************************************************************************/
EXPORT void wait_10us(void);

/******************************************************************************/
/*            U N T E R P R O G R A M M:                                      */
/*                                                                            */
/* Aufgabe: Function converts a Nibble(0-F) to an ASCII ('0'-'F')             */
/* Input:   nib: Nibble to convert                                            */
/* Output:  nib: Converted Nibble                                             */ 
/* return:	-                                                                 */
/******************************************************************************/
EXPORT void nib2asc(char *nib);

/******************************************************************************/
/*            U N T E R P R O G R A M M:                                      */
/*                                                                            */
/* Aufgabe: Function converts an ASCII('0'-'F') to a Nibble(0-F)              */
/* Input:   asc: ASCII Code                                                   */
/* Output:  asc: Hex Value                                                    */ 
/* return:	- 	                                                              */
/******************************************************************************/
EXPORT void asc2nib(char *asc);

/******************************************************************************/
/*            U N T E R P R O G R A M M:                                      */
/*                                                                            */
/* Aufgabe: Function converts a Hex-Code (00-FF) to an BCD (0-255)            */
/* Input:   hex: Hex Value                                                    */
/* return:	BCD Value                                                         */
/******************************************************************************/
EXPORT int hex2bcd(char hex);


/* ------------------------- UART Funktionen ----------------------------------*/


/******************************************************************************/
/*            U N T E R P R O G R A M M:                                      */
/*                                                                            */
/* Aufgabe: Initialisiert UART1                                               */
/* Input:   Baudrate                                                          */
/* return:	 	                                                              */
/******************************************************************************/
EXPORT void uart_init(unsigned long baudrate);

/******************************************************************************/
/*            U N T E R P R O G R A M M:                                      */
/*                                                                            */
/* Aufgabe: liest ein Zeichen von UART1 ein                                   */
/* Input:                                                                     */
/* return:	eingelesens Zeichen                                               */
/******************************************************************************/
EXPORT char uart_get_char(void); 

/******************************************************************************/
/*            U N T E R P R O G R A M M:                                      */
/*                                                                            */
/* Aufgabe: Ausgabe eines Zeichens auf UART1                                  */
/* Input:   ch: Zeichen in ASCII Code                                         */
/* return:	 	                                                              */
/******************************************************************************/
EXPORT void uart_put_char(char ch);

/******************************************************************************/
/*            U N T E R P R O G R A M M:                                      */
/*                                                                            */
/* Aufgabe: Ausgabe eines 8 Bit Hex Wertes als ASCII String auf UART1         */
/* Input:                                                                     */
/* return:	 	                                                              */
/******************************************************************************/
EXPORT void uart_put_hex(char c); 

/******************************************************************************/
/*            U N T E R P R O G R A M M:                                      */
/*                                                                            */
/* Aufgabe: Ausgabe eines Strings auf UART1                                   */
/* Input:   string: C- String der aud UART1 ausgegeben werden soll            */
/* return:	 	                                                              */
/******************************************************************************/
EXPORT void uart_put_string(char *string);

/******************************************************************************/
/*            U N T E R P R O G R A M M:                                      */
/*                                                                            */
/* Aufgabe: sendet ein CLEAR Screen String ("ESC[2J") zu einem VT100 Terminal */
/* Input:                                                                     */
/* return:	 	                                                              */
/******************************************************************************/
EXPORT void uart_clear(void);

/******************************************************************************/
/*            U N T E R P R O G R A M M:                                      */
/*                                                                            */
/* Aufgabe:  sendet Positionierungsstring auf VT100 Terminal "ESC[y;xH"       */
/* Input:                                                                     */
/* return:	 	                                                              */
/******************************************************************************/
EXPORT void uart_setpos(char x,char y);

/* ------------------------- LCD Funktionen ----------------------------------*/

/*******************************************************************************
* Initialize the LCD controller                                                *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/
EXPORT void lcd_init (void);

/*******************************************************************************
* Write command to LCD controller                                              *
*   Parameter:    c:      command to be written                                *
*   Return:                                                                    *
*******************************************************************************/
EXPORT void lcd_write_cmd (unsigned char c);

/*******************************************************************************
* Write data to LCD controller                                                 *
*   Parameter:    c:      data to be written                                   *
*   Return:                                                                    *
*******************************************************************************/
EXPORT void lcd_write_data (unsigned char c);

/******************************************************************************/
/*            U N T E R P R O G R A M M:                                      */
/*                                                                            */
/* Aufgabe: Print Character to current cursor position                        */
/* Input:   c:      character to be printed                                   */
/* return:	 	                                                              */
/******************************************************************************/
EXPORT void lcd_put_char (char c);

/******************************************************************************/
/*            U N T E R P R O G R A M M:                                      */
/*                                                                            */
/* Aufgabe: Ausgabe eines 8 Bit Hex Wertes als ASCII String auf LCD           */
/* Input:   c: Hex Value to be printed                                        */
/* return:	 	                                                              */
/******************************************************************************/
EXPORT void lcd_put_hex(char c); 


/*******************************************************************************/
/* Set cursor position on LCD display                                          */
/*   Parameter: line:   line position (0-3)                                    */
/*              column: column position (0-15)                                 */
/*   Return:                                                                   */
/*******************************************************************************/
EXPORT void lcd_set_cursor (int line_nr, int column_nr);

/*******************************************************************************
* Clear the LCD display                                                        *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/
EXPORT void lcd_clear (void);

/*******************************************************************************
* Print sting to LCD display                                                   *
*   Parameter:    string: pointer to output string                             *
*   Return:                                                                    *
*******************************************************************************/
EXPORT void lcd_put_string (char *string);

/*******************************************************************************
* Print a bargraph to LCD display                                              *
*   Parameter:     val:  value 0..100 %                                        *
*                  size: size of bargraph 1..16                                *
*   Return:                                                                    *
*******************************************************************************/
EXPORT void lcd_bargraph (int value, int size);

/*******************************************************************************
* Display bargraph on LCD display                                              *
*   Parameter:     pos_y: vertical position of bargraph                        *
*                  pos_x: horizontal position of bargraph start                *
*                  value: size of bargraph active field (in pixels)            *
*   Return:                                                                    *
*******************************************************************************/
EXPORT void lcd_bargraphXY (int pos_y, int pos_x, int value);



/* ------------------------- ADC Funktionen ----------------------------------*/

/******************************************************************************/
/*            U N T E R P R O G R A M M:    adc1_convert                      */
/*                                                                            */
/* Aufgabe:  liefert aktuellen Wert von ADC1 für Kanal channel                */
/*                                                                            */
/* Input:  channel to convert                                                 */                                                               
/* return:	converted value (12Bit right aligned)                             */
/******************************************************************************/
EXPORT unsigned short int adc1_convert(unsigned char channel);


/******************************************************************************/
/*            U N T E R P R O G R A M M:    ton                               */
/*                                                                            */
/* Aufgabe:  Wiedergabe eines Ton auf Piezo Summer                            */
/*                                                                            */
/* Input:   frequenz: Frequenz des Tons in Hz                                 */                                                               
/*          dauer:    Dauer des in ms                                         */
/* return:	                                                                  */
/******************************************************************************/
EXPORT void ton(int frequenz,int dauer_ms);

/******************************************************************************/
/*            U N T E R P R O G R A M M:    set_clock_72MHz                   */
/*                                                                            */
/* Aufgabe:  sets System Clock to 72MHz                                       */
/*                                                                            */
/* Input:                                                                     */                                                               
/* return:	                                                                  */
/******************************************************************************/
EXPORT void set_clock_72MHz(void);




#undef  EXPORT
#endif /* __ARMV10_STD_H */

/******************* (C) HTL - HOLLABRUNN  2009-2010 *****END OF FILE****/
