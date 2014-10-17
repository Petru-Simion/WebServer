#include "LCD.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "clock.h"

#define wait_interval 5

static uint8_t setup = false;
static uint8_t wait_timer = 0;
static uint8_t s;
static uint8_t m;
static uint8_t h;
static uint8_t i = 0;

static uint8_t digit_array[6]; 

/***********************/
//! Update display
/***********************/

void display_update (void)
{
   char time_str[8]; 

   LCD_goto(1,1); 
   sprintf(time_str, "%02d:%02d:%02d", h, m, s);
   LCD_str(time_str);
}

/***********************/
//! Set clock time
//! Input: 0-9 and *
/***********************/

void SetClock(char key)
{  
   // Timer to avoid multiple key-presses:
   if (wait_timer > 0)
   {
      wait_timer--;
   }

   // Enter/leave setup:
   if (key == '*' && wait_timer == 0)
   {
      setup = !setup;
      wait_timer = wait_interval;
      
       // Init array and counter:
      if (setup == true)
      {
         i = 0;
        
         for (int p = 0; p < 6; p++)
         {
            digit_array[p] = 0;
         }
      }

      // Update clock when leaving setup, 
      // if a new time has been entered.
      // Else continue from previous time:
      else
      {
         if (i > 0)
         {
            clockUpdate(h, m, s);
         }
      }
   }
 
   switch (setup)
   {
      // clock stopped:
      case true:
      clock_master = false;  

      // Check for valid input and add key to array:
      if (key != 0 && key != '*' && key != '#' && wait_timer == 0)        
      {
         wait_timer = wait_interval;        
         digit_array[i] = (key - 48);        
         i++;
      } 
      
      // Convert digits to time and write to display:   
      if (i > 0)
      {
         h = ((digit_array[0] * 10)+digit_array[1]);
         m = ((digit_array[2] * 10)+digit_array[3]);
         s = ((digit_array[4] * 10)+digit_array[5]);

         display_update();

         // Limit digits to 24-hour format:
         if (h > 23)
         {
            h = 23;               
         }

         if (m > 59)
         {
            m = 59;
         }

         if (s > 59)
         {
            s = 59;      
         }
      }

      // If no digits entered:
      else
      { 
         LCD_goto(1,1);
         LCD_str("--:--:--");
      }

      // Auto-update clock after 6 digits:
      if (i == 6)
      {   
         clockUpdate(h, m, s); 
         
         // Exit setup:
         setup = false;           
      }           
      break;
      
      // Clock running: 
      case false:      
      clock_master = true;
      break;
   }
}
