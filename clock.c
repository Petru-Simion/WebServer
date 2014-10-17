#include <stdio.h>
#include <stdbool.h>
#include "LCD.h"

static char seconds;
static char minutes;
static char hours;
static char save[8];

int clock_master = true;
bool blink;
char sep_sign = ':';

/**********************************/
// Display time
/**********************************/

void clock(void)
{           
   if (clock_master == true) // Clock running
   {
      LCD_goto(1,1); 
      sprintf(save, "%02d%c%02d%c%02d", 
      hours, sep_sign, minutes, sep_sign, seconds);
      LCD_str(save);  

      seconds++;

      if(seconds == 60)
      {
         seconds = 0;           
         minutes++;
      }
      
      if(minutes == 60)
      {
         minutes = 0;
         hours++;                  
      }
      
      if(hours == 24)
      {
         hours=0;                  
      } 

      blink = !blink;

      if (blink == true)
      {
         sep_sign = ':';
      }
      else
      {
         sep_sign = 32; 
      }
   }
}

void clockUpdate(char h, char m, char s)        
{    
   // Update the clock with the entered time:                          
   hours = h;                   
   minutes = m;
   seconds = s;
}
