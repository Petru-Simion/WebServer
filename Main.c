#define F_CPU 16000000UL // 16 MHz clock speed

#define BAUD 9600
#define BAUDRATE (F_CPU/16/BAUD)-1

#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ChangeTemp.h"
#include "clock.h"
#include "LCD.h"
#include "ADC.h"
#include "LCD_fixed_text.h"
#include "Interrupts.h"
#include "Keyboard.h"
#include "SetClock.h"
#include "BarGraph.h"
#include "usart.h"
#include "lowlevel.h"
#include "enc28j60.h"
#include "ip_arp_udp_tcp.h"
#include "net.h"
#include "webserver.h"
#include "TypeConversion.h"

static char key;
static int LDR;
static char ReceivedData;
int time_count;

/***********************/
// Main Program
/***********************/

int main (void)
{  
	LCD_init();
	LCD_cmd(0x00); // Blink cursor
   	LCD_bars_init(); // Store bargraph characters to LCD
  
	usart_init(BAUDRATE);

	fixed_text();
	timer1_init();

   // LAN init
   InitPhy();		
   extInterruptInit();

	while(1)
	{ 
	  if(WebTimer_run)
	  {
	    time_count ++;
	  	WebTimer_run = false;
	  }

 	  if (int28j60)
      {
         int28j60 = false;
         LanTask();
      }

      // Function run flags:
      if(clock_run)
      {  
         clock();       
         clock_run = false;
      }   

      if(SetClock_run)
      {
         SetClock(key);
         SetClock_run = false;
      }

      if(LDR_run)   
      {  
         LDR = light();        
         LDR_run = false;
      }

      if(temp_run)
      { 
         temp();
         temp_run = false;
      }

      if(key_run)
      {  
         key = KEY_read();
         key_run = false;
      }

       if(backlight_run)
      {  
         if (ReceivedData == 'A')
         {
            BL_dimmer('#', LDR);
         }

         BL_dimmer(key, LDR);

         backlight_run = false; 
      }

      if (ChangeTemp_run)
      { 
         if (clock_master == true)
         {
            ChangeTemp(key);
            ChangeTemp_run = false;
         }         
      }

      if (LCD_bars_run)
      { 
         LCD_bars(LDR);
         LCD_bars_run = false;
      }


     if (USART_TX_run)
      { 
         usart_putstr(intToString(getLDR()));
         USART_TX_run = false;
      }

      if (USART_RX_run)
      { 
		 ReceivedData = usart_rx();
         USART_RX_run = false;
      }
   }
}

