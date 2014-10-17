#define F_CPU 16000000UL // 16 MHz clock speed
#include <avr/io.h>
#include <util/delay.h>

/***************************/
// Initialize rows for read
/***************************/

void rows_init()
{  
   DDRD &= 0b10000111;   //set rows as input
   PORTD |= 0b11111000;  // pull up resistors
   DDRB |= 0b10000111;   // set columns as output
   PORTB &= 0b11111000;  // set colummns low
   _delay_us(1);
}

/*****************************/
// Initialize columns for read 
/*****************************/

void col_init()
{
   DDRB &= 0b11111000;   //set columns as input
   PORTB |= 0b10000111;  // pull up resistors
   DDRD |= 0b11111000;   // set rows as output
   PORTD &= 0b10000111;  // set rows low
   _delay_us(1);
}

/************************************/
// Initialize Keyboard
// Output: Returns keyboard input
/************************************/

char KEY_read()
{ 
   unsigned char curr_char = 0;
   static unsigned char prev_char = 'Z'; // dummy
   int y,x,z;

   rows_init();
   y = PIND | 0b10000111; //read row inputs 

   col_init();
   x = PINB | 0b11111000; //read column inputs
  
   z = x & y; 

   switch (z)
   {   
      // Convert input to character:
      case 0b10111110:
      curr_char = ('*');
      break;

      case 0b10111011:
      curr_char = ('#');
      break;

      case 0b10111101:
      curr_char = ('0');
      break;

      case 0b11011110:
      curr_char = ('7');
      break;

      case 0b11011101:
      curr_char = ('8');
      break;

      case 0b11011011:
      curr_char = ('9');
      break;

      case 0b11101110:
      curr_char = ('4');
      break;

      case 0b11101101:
      curr_char = ('5');
      break;

      case 0b11101011:
      curr_char = ('6');
      break;

      case 0b11110110:
      curr_char = ('1');
      break;

      case 0b11110101:
      curr_char = ('2');
      break;

      case 0b11110011:
      curr_char = ('3');
      break; 
   }

   // Return key only after two identical key reads:
   if (curr_char == prev_char)
   {   
      prev_char = curr_char;  
      return curr_char; 
   }
   else
   {  
      prev_char = curr_char;
      return 0;    
   }     
}
