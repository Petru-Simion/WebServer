#include <avr/io.h>


//*****************************************************************************
extern void InitIo (void)
{
   DDRA = 0xFF;
   DDRB = 0xFF;
   DDRC = 0xFF;
   DDRD = 0xFF;
   PORTA = 0xFF;
   PORTB = 0xFF;
   PORTC = 0xFF;
   PORTD = 0xFF;
}


//*****************************************************************************
extern void DelayUs (uint8_t us)
{
   volatile char dummy = 0;

   while (us) 
   {
      us--;
      dummy++;
   }
}


//*****************************************************************************
extern void DelayMs (uint16_t ms)
{
   volatile int i;

   while (ms > 0)                      // count the number of ms
   {
      for (i = 0; i < 857; i++);       // found by test, = 1 ms @ 16 MHz
      ms--;
   }
}
