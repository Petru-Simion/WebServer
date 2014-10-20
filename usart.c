/********************************************************************************
Includes
********************************************************************************/
#define F_CPU 16000000UL 
#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <stdlib.h>				// itoa conversion
#include "LCD.h"

/********************************************************************************
Macros and Defines
********************************************************************************/
#define BAUD 9600
#define BAUDRATE (F_CPU/16/BAUD)-1
#define BUFFERSIZE 100

uint8_t txBuffer[BUFFERSIZE], rxBuffer[BUFFERSIZE];
uint8_t TxRd = 0, TxWr = 0, RxRd = 0, RxWr = 0;
bool RxComplete = false;
bool TxComplete = false;
char rxData = 0;

/********************************************************************************
Function Prototypes
********************************************************************************/
void usart_init(uint16_t ubrr);
char usart_getchar( void ); 
void usart_putchar( char data ); 
void usart_putstr (char *s);
void usart_tx(char txByte);
char usart_rx();

/********************************************************************************
Transmit via Buffers
********************************************************************************/
void usart_tx(char txByte)
{
	txBuffer[TxWr] = txByte; 
	TxWr++;

	if(TxWr == BUFFERSIZE)
	{
		TxWr = 0;
	}
}
 
/********************************************************************************
Receive via Buffers
********************************************************************************/
char usart_rx()
{	
	rxData = 0;
 	if (RxComplete == true) // get data if TX complete and increment buffer
	{
		rxData= rxBuffer[RxRd]; 
		RxRd++;
	}
	
   if (RxRd == RxWr) 
	{	
		RxComplete = false; // reset flag if all data has been read
	}

	if(RxRd == BUFFERSIZE)
	{
		RxRd = 0; // reset buffer if reached end
	}

	return(rxData);
}

/********************************************************************************
USART Functions
********************************************************************************/
// Init the UART
void usart_init( uint16_t ubrr) 
{
    // Set baud rate
    UBRRH = (uint8_t)(ubrr>>8);
    UBRRL = (uint8_t)ubrr;

	 //UCSRA = 0b01100010;	Just added
    			
    // Enable  Rx interrupt, Tx interrupt, receiver and transmitter
    UCSRB = (1<<RXCIE)|(1<<TXCIE)|(1<<RXEN)|(1<<TXEN);

    // Set frame format: 8data, 1stop bit
    UCSRC = (1<<URSEL)|(3<<UCSZ0);
    DDRD = 0b10000010; 
    PORTD = 0b00000010;
    
	 UDR =' '; // start first TX manually
}

// Write a string to the tx buffer 
void usart_putstr(char *s) 
{
    // loop through entire string
    while (*s) 
	{ 
        usart_tx(*s);
        s++;
   	}
    UDR = '\n';
}

// INTERRUPTS //

// Fires when receive is complete
ISR (USART_RXC_vect)
{  
	rxBuffer[RxWr] = UDR;
	RxWr++;

	if (RxWr == BUFFERSIZE)
	{
		RxWr = 0;
	}
	RxComplete = true;
}

// Fires when transmit is complete
ISR (USART_TXC_vect)
{  
	if (TxWr != TxRd)
		{
			UDR = txBuffer[TxRd];
			TxRd++;
		}

	if (TxRd == BUFFERSIZE)
		{
			TxRd = 0;
		}
	TxComplete = true;
}
