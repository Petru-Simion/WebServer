#include <avr/io.h>
#include <stdio.h>
#include "ADC.h"
#include <stdbool.h>

int temp_master;
int wait_interval = 5;
int wait_timer = 0;
int state = 0;

/**************************************/
// Switch temperature between C or Fahr.
// Input: Key '*' toggles state
/**************************************/

void ChangeTemp (char key)
{   
   // Timer to avoid multiple key presses
   if (wait_timer > 0)  
   {      
      wait_timer--;
   }

   if (key == '0' && wait_timer == 0)
   {
      state = !state;
      wait_timer = wait_interval;
   }

   if (temp_master == true)
   {
      // Alternate between Celsius & Fahrenheit
      switch (state)                  
      { 
         case 0:
         celsius();
         break;

         case 1:
         fahrenheit();
         break;
      }    
   }
}
