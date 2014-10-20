#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "LCD.h"
#include "enc28j60.h"

// RTOS timer intervals:

#define clock_interval 995; // Calibrated value for 1s
#define SetClock_interval 20;      
#define LDR_interval 150; 
#define temp_interval 50; // Averaged 10 times = 500ms
#define key_interval 20;
#define backlight_interval 20;
#define ChangeTemp_interval 20;
#define LCD_bars_interval 200;
#define USART_RX_interval 50;
#define USART_TX_interval 100;
#define webserver_interval 20;

// RTOS timer variables: 

int clock_run;
int clock_count;

int SetClock_run;
int SetClock_count;

int LDR_run;
int LDR_count;

int temp_run;
int temp_count;

int key_run;
int key_count;

int backlight_run;
int backlight_count;

int ChangeTemp_run;
int ChangeTemp_count;

int LCD_bars_run;
int LCD_bars_count;

//USART
int USART_TX_run;
int USART_TX_count;

int USART_RX_run;
int USART_RX_count;

//LAN
int int28j60 = false;
uint16_t isrCount = 0;

// Initialize timer, interrupt and variable
void timer1_init()
{   
   // Set up timer with prescaler = 64 and CTC mode
   TCCR1B |= (1 << CS11) | (1 << CS10) | (1 << WGM12);

   // Initialize counter
   TCNT1 = 0;

   // Initialize compare value
   OCR1A = 250;    // 1ms = 16.000Hz / 64 = 250 cycles

   // Enable compare interrupt
   TIMSK |= (1 << OCIE1A);

   // Enable global interrupts
	sei();
}

void extInterruptInit()
{
	//Initialize the external interrupt (INT0)
	GICR |= 0x40;
	MCUCR = 0x02;
	MCUCSR = 0x00;
	GIFR = 0x40;
}
// External timer
ISR(INT0_vect)
{
   int28j60 = true;   
   isrCount++;
}

// This ISR is fired whenever a match occurs
ISR (TIMER1_COMPA_vect)
{  
   if(clock_count-- == 0)
   { 
      clock_count = clock_interval;
      clock_run = true;
   } 

    if(SetClock_count-- == 0)
   { 
      SetClock_count = SetClock_interval;
      SetClock_run = true;
   } 

   if(LDR_count-- == 0)
   { 
      LDR_count = LDR_interval;
      LDR_run = true;
   } 

   if(temp_count-- == 0)
   { 
      temp_count = temp_interval;
      temp_run = true;
   } 

   if(key_count-- == 0)
   { 
      key_count = key_interval;
      key_run = true;
   } 

   if(backlight_count-- == 0)
   { 
      backlight_count = backlight_interval;
      backlight_run = true;
   }          
   if(ChangeTemp_count-- == 0)
   { 
      ChangeTemp_count = ChangeTemp_interval;
      ChangeTemp_run = true;
   } 
   
   if(LCD_bars_count-- == 0)
   { 
      LCD_bars_count = LCD_bars_interval;
      LCD_bars_run = true;
   }    
   if(USART_RX_count-- == 0)  
   {
      USART_RX_count = USART_RX_interval;
      USART_RX_run = true;
   }      
   
   if(USART_TX_count-- == 0)  
   {
      USART_TX_count = USART_TX_interval;
      USART_TX_run = true;
   }        
}
