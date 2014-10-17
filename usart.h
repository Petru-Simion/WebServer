#ifndef usart_h
#define usart_h 


extern char usart_rx();
extern void usart_tx(char txByte);
extern void usart_putstr(char *s);
extern void usart_init(uint16_t ubrr);

#endif
