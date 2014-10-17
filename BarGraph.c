#include <stdbool.h>

#include "LCD.h"
#include "Interrupts.h"

/**********************************/
// Bar Graph Calculation and Print
// Input: Bargraph value
/**********************************/

void LCD_bars(int x)
{
   //volatile 
   char bars[7];
   long steps;
  
   // Divide ADC into 35 steps and multiply/divide 
   // by 10 to avoid floating point issues:
   steps = ((x / 29 ) * 10);

   // Calculate active lines for Bar A-G

   // Bar A:
   if ((steps/5) >=10)   
   {
      bars[0] = 5;
   }
  
   else if ((steps/5) <= 0)
   {
      bars[0] = 0;
   }

   else
   {
      bars[0] = (steps % 50) / 10;
   }

   // Bar B:   
   if (((steps-50)/5) >= 10)   
   {
      bars[1] = 5;
   }
  
   else if (((steps-50)/5) <= 0)
   {
      bars[1] = 0;
   }

   else
   {
      bars[1] = ((steps-50) % 50) / 10;
   }

   // Bar C:
   if (((steps-100)/5) >= 10)   
   {
      bars[2] = 5;
   }
  
   else if (((steps-100)/5) <= 0)
   {
      bars[2] = 0;
   }

   else
   {
      bars[2] = ((steps-100) % 50) / 10;
   }

   // Bar D:
   if (((steps-150)/5) >= 10)   
   {
   bars[3] = 5;
   }
  
   else if (((steps-150)/5) <= 0)
   {
   bars[3] = 0;
   }

   else
   {
      bars[3] = ((steps-150) % 50) / 10;
   }

   // Bar E
   if (((steps-200)/5) >= 10)   
   {
      bars[4] = 5;
   }
  
   else if (((steps-200)/5) <= 0)
   {
      bars[4] = 0;
   }

   else
   {
      bars[4] = ((steps-200) % 50) / 10;
   }

   // Bar F
    if (((steps-250)/5) >= 10)   
   {
      bars[5] = 5;
   }
  
   else if (((steps-250)/5) <= 0)
   {
      bars[5] = 0;
   }

   else
   {
      bars[5] = ((steps-250) % 50) / 10;
   }

   // Bar G:
   if (((steps-300)/5) >= 10)   
   {
      bars[6] = 5;
   }     

   else if (((steps-300)/5) <= 0)
   {
      bars[6] = 0;
   }
 
   else
   {
      bars[6] = ((steps-300) % 50) / 10;
   }

   // Print bargraph on LCD:
   for (int i = 0; i < 7; i++)
   {
      LCD_goto(14+i,2);
      LCD_prt(bars[i]);
   } 
}


