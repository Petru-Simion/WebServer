#define F_CPU 16000000UL // 16 MHz clock speed
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include "BitManip.h"
#include "Interrupts.h"
#include "ADC.h"
#include "lowlevel.h"
#include "enc28j60.h"
#include "ip_arp_udp_tcp.h"
#include "net.h"
#include "webserver.h"

#include <stdint.h>
#include <stdio.h>

/***********************/
// LCD Pin Definitions 
/***********************/

#define CS 0x10   // CS  Used to start the transmission for potentiometer 
#define RS 0x04 	// LCD Register Select on PA2
#define RW 0x08  	// LCD RW on PA3
#define D0 0x10 	// LCD Data Pin 0 on PA4 
#define D1 0x20 	// LCD Data Pin 1 on PA5
#define D2 0x40 	// LCD Data Pin 2 on PA6
#define D3 0x80  	// LCD Data Pin 3 on PA7
#define E  0x40 	// LCD E on PC6
#define BL 0x80	// LCD Backlight on PD7 (via transistor switch)

/***********************/
// Variable Definitions
/***********************/

int LCD_busy;
int useBusy;
int count;

/********************************/
// LCD Command Write
// Input: LCD command data
/********************************/

void LCD_cmd (char x)
{
   int dummy = 0; // Used for small delay

   // Split 8-bit command into 2 bytes with 4 bits each
   char MSB = x & 0xF0; // mask out LSB;
   char LSB = x << 4; // move LSB up to MSB

   // Set RS = 0 to send a command to LCD RS = 1 to send an ASCII value
   bit_clr(PORTA, RS);

   // Set RW = 0 to write to LCD, RW = 1 to read from LCD
   bit_clr(PORTA, RW);

   // Set E = 1
   bit_set(PORTC, 0x40);

   // Send MSB on DB4-7
   // PORTA = (PORTA & 0x0F) | MSB
   PORTA &= 0x0F; // Clear MSB bits
   PORTA |= MSB;


   // Set E = 0
   bit_clr(PORTC, E);

   dummy++; // Small delay

   // Set E = 1
   bit_set(PORTC, E);

   // Send LSB on DB4-7
   PORTA &= 0x0F; // Clear MSB bits
   PORTA |= LSB;

   // Set E = 0
   bit_clr(PORTC, E);

   _delay_ms(2);
}

/***********************/
// LCD Char Print
// Input: ASCII value
/***********************/

void LCD_prt (char x)
{
   int dummy = 0; // Used for small delay

   // Split 8-bit command into 2 bytes with 4 bits each
   char MSB = x & 0xF0; // mask out LSB;
   char LSB = x << 4; // move LSB up to MSB

   // Set RS = 0 to send a command to LCD RS = 1 to send an ASCII value
   bit_set(PORTA, RS);

   // Set RW = 0 to write to LCD, RW = 1 to read from LCD
   bit_clr(PORTA, RW);

   // Set E = 1
   bit_set(PORTC, 0x40);

   // Send MSB on DB4-7
   // PORTA = (PORTA & 0x0F) | MSB
   PORTA &= 0x0F; // Clear MSB bits
   PORTA |= MSB;

   // Set E = 0
   bit_clr(PORTC, E);

   dummy++; // Small delay

   // Set E = 1
   bit_set(PORTC, E);

   // Send LSB on DB4-7
   PORTA &= 0x0F; // Clear MSB bits
   PORTA |= LSB;

   // Set E = 0
   bit_clr(PORTC, E);

   _delay_ms(2);
}

/***********************/
// Write to LCD CGRAM
// Input: Hex data 
/***********************/

void LCD_gen (char x)
{
 	int dummy = 0; // Used for small delay

	// Split 8-bit command into 2 bytes with 4 bits each
	char MSB = x & 0xF0; // mask out LSB;
	char LSB = x << 4; // move LSB up to MSB

	// Set RS = 0 to send a command to LCD RS = 1 to send an ASCII value
	bit_set(PORTA, RS);
	
	// Set RW = 0 to write to LCD, RW = 1 to read from LCD
	bit_clr(PORTA, RW);

	// Set E = 1
	bit_set(PORTC, 0x40);

	// Send MSB on DB4-7
	// PORTA = (PORTA & 0x0F) | MSB
	PORTA &= 0x0F; // Clear MSB bits
	PORTA |= MSB;
	

	// Set E = 0
	bit_clr(PORTC, E);

	dummy++; // Small delay

	// Set E = 1
	bit_set(PORTC, E);
	
	// Send LSB on DB4-7
	PORTA &= 0x0F; // Clear MSB bits
	PORTA |= LSB;
	
	// Set E = 0
	bit_clr(PORTC, E);
	
	_delay_ms(2);
}

/***********************/
// Read from LCD
/***********************/

void LCD_read (char x)
{
 	int dummy = 0; // Used for small delay

	// Split 8-bit command into 2 bytes with 4 bits each
	char MSB = x & 0xF0; // mask out LSB;
	char LSB = x << 4; // move LSB up to MSB

	// Set RS = 0 to send a command to LCD RS = 1 to send an ASCII value
	bit_set(PORTA, RS);
	
	// Set RW = 0 to write to LCD, RW = 1 to read from LCD
	bit_set(PORTA, RW);

	// Set E = 1
	bit_set(PORTC, 0x40);

	// Send MSB on DB4-7
	// PORTA = (PORTA & 0x0F) | MSB
	PORTA &= 0x0F; // Clear MSB bits
	PORTA |= MSB;
	
	// Set E = 0
	bit_clr(PORTC, E);

	dummy++; // Small delay

	// Set E = 1
	bit_set(PORTC, E);
	
	// Send LSB on DB4-7
	PORTA &= 0x0F; // Clear MSB bits
	PORTA |= LSB;
	
	// Set E = 0
	bit_clr(PORTC, E);
	
	_delay_ms(2);
}

/***********************************/
// Store bargraph chars in LCD
/***********************************/

void LCD_bars_init (void)
{
   // store 1 horizontal line to CGRAM addr. 0:
   LCD_cmd(0x40);
   LCD_gen(0x00);
   LCD_gen(0x00);
   LCD_gen(0x00);
   LCD_gen(0x00);
   LCD_gen(0x00);
   LCD_gen(0x00);
   LCD_gen(0x00);
   LCD_gen(0x1F);
	
   // Store 1 vertical lines to CGRAM addr. 1:
   LCD_cmd(0x48);
   LCD_gen(0x10);
   LCD_gen(0x10);
   LCD_gen(0x10);
   LCD_gen(0x10);
   LCD_gen(0x10);
   LCD_gen(0x10);
   LCD_gen(0x10);
   LCD_gen(0x1F);

   // Store 2 vertical lines to CGRAM addr. 2:
   LCD_cmd(0x50);
   LCD_gen(0x18);
   LCD_gen(0x18);
   LCD_gen(0x18);
   LCD_gen(0x18);
   LCD_gen(0x18);
   LCD_gen(0x18);
   LCD_gen(0x18);
   LCD_gen(0x1F);

   // Store 3 vertical lines to CGRAM addr. 3:
   LCD_cmd(0x58);
   LCD_gen(0x1C);
   LCD_gen(0x1C);
   LCD_gen(0x1C);
   LCD_gen(0x1C);
   LCD_gen(0x1C);
   LCD_gen(0x1C);
   LCD_gen(0x1C);
   LCD_gen(0x1F);

   // store 4 vertical lines to CGRAM addr. 4:
   LCD_cmd(0x60);
   LCD_gen(0x1E);
   LCD_gen(0x1E);
   LCD_gen(0x1E);
   LCD_gen(0x1E);
   LCD_gen(0x1E);
   LCD_gen(0x1E);
   LCD_gen(0x1E);
   LCD_gen(0x1F);

   // store 5 vertical lines to CGRAM addr. 5:
   LCD_cmd(0x68);
   LCD_gen(0x1F);
   LCD_gen(0x1F);
   LCD_gen(0x1F);
   LCD_gen(0x1F);
   LCD_gen(0x1F);
   LCD_gen(0x1F);
   LCD_gen(0x1F);
   LCD_gen(0x1F);
}

/***********************/
// LCD Initialization
/***********************/

void LCD_init (void)
{  
   /* uP Pin definition */
   DDRA = 0xFC; // Pin 2-7: LCD RS, RW, D0-D3
	DDRC = 0x40; // Pin 7: LCD E
	DDRD = 0xFF; //0x80; // Pint 8: LCD Backlight
   
   /* LCD hardware initialization */
	_delay_ms(100);
	LCD_cmd(0x28);
	_delay_ms(10);	
	LCD_cmd(0x28);
	LCD_cmd(0x28);

	LCD_cmd(0x06);
	LCD_cmd(0x0C);
	LCD_cmd(0x01);	
}

/*******************************/
// LCD Backlight On/Off
// Input: 0 (BL off) or 1 (BL on)
/*******************************/

void LCD_BL(int x)
{
	switch(x)
	{
   	case 0: // Turn off backlight
   	bit_clr(PORTD, BL);
   	break;

   	case 1: // Turn on backlight
   	bit_set(PORTD, BL);
   	break;
	}
}

/**************************/
// LCD Print String
// Input: Char String
// Output: Character
/**************************/

void LCD_str(char *str)
{
   // Print char and increment counter until EOS
  	while(*str > 0)
 	{	
      LCD_prt(*str++); 
   }
}

/***********************/
// LCD Move left or right
// Input: 0 or 1
/***********************/

void LCD_move(int m)
{
	switch(m)	
	{
   	case 0:
   	LCD_cmd(0x10); // Move cursor to the left
   	break;
	
   	case 1:
   	LCD_cmd(0x14); // Move cursor to the right
   	break;
	}
}
/**********************************/
// LCD Goto XY Position
// Input: X pos.: 1-20, Y pos.: 1-2
/**********************************/

void LCD_goto(int x, int y)
{
	switch(y)
	{
      // Goto x value on first line
   	case 1: 
   	LCD_cmd(0x80| (x-1));
   	break;

      // Goto x value on second line
   	case 2:
   	x += 40;
   	LCD_cmd(0x80| (x-1));
   	break;
	}
}

/***********************/
// LCD Display Clear
/***********************/

void LCD_clr()
{
   LCD_cmd(0x01); // Clears LCD and returns cursor to home
}

/**********************************************************/
// LCD Backlight Dimmer
// Input: Key pressed, LDR value, BL_autodim_master state) 
/**********************************************************/

void BL_dimmer (char key, int LDR)
{
   static char BL_dimValue = 255;
   static int toggle = 1;
   static char wait_timer = 0;
   int Light;
   
   const static char wait_interval = 10;
   #define fade_speed 15
   #define test_max 240
   #define test_min 15

   // Setup TIMER2 with no prescaler and fast PWM mode
   TCCR2 |= (1 << WGM20)|(1 << WGM21)|(1 << CS20)|(1 << COM21);

   // Timer to avoid multiple key-presses:
   if (wait_timer > 0)  
   {
      wait_timer--;
   }

   // Toggles backlight on, off or auto (LDR): 
   if (key == '#' && wait_timer == 0)
   {
      if (toggle >= 2)
      {
         toggle = 0;
      }
      else 
      {
         toggle++;
      }
     
      wait_timer = wait_interval;
   }

   switch (toggle)
   {
      case 0:
      
      LCD_goto(10,2);
      LCD_str("OFF");

      // Fade-out LCD BL:
      if (BL_dimValue > 0)
      {
         if (BL_dimValue <= test_min)
         {
            BL_dimValue = 0;
         }
         else 
         {
            BL_dimValue -= fade_speed;
         }
      }
      break;

      case 1:
      LCD_goto(10,2);
      LCD_str("ON ");

      // Fade-in LCD BL:
      if (BL_dimValue < 255)
      {
         if (BL_dimValue >= test_max)
         {
            BL_dimValue = 255;
         }
         else 
         {
            BL_dimValue += fade_speed;
         }
      }  
      break;

      case 2:
      // Convert LDR value to 8 bit:
      Light = (LDR / 4);

      // Dim LCD backlight based on LDR value.
      // In the deadband between 50-60 and 235-245,
      // the previous value is retained in
      // order to avoid flickering when 
      // changing between max, min and variable
      // backlight intensity:

      if (Light >= 245)   
      {
         BL_dimValue = 255;
      }
      else if (Light <= 50)
      {
         BL_dimValue = 50;
      }
      else if (Light < 235 && Light > 60)
      {
         BL_dimValue = Light;
      } 

      LCD_goto(10,2);
      LCD_str("LDR");
      
      break;
   }

   // PWM width for fade-in/out:  
   OCR2 = BL_dimValue; 
   
    
}


