#include "LCD.h"
#include <stdio.h>

/***********************/
// LCD - Fixed Text
/***********************/

void fixed_text ()
{
   LCD_goto(11,1);
   LCD_str("T:");

   LCD_goto(15,1);
   LCD_str(".");

   LCD_goto(1,2);
   LCD_str("LDR:");
   
 
   LCD_goto(18,1);
   LCD_prt((char)223); // Degree sign              

}
