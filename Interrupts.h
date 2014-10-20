#ifndef Interrupts_h
#define Interrupts_h               

extern void timer1_init();
extern void extInterruptInit();
extern int clock_run;
extern int SetClock_run;
extern int LDR_run;
extern int temp_run;
extern int key_run;
extern int backlight_run;
extern int ChangeTemp_run;
extern int LCD_bars_run;
extern int USART_RX_run;
extern int USART_TX_run;
extern int int28j60;
extern int webserver_run;
extern int isrCount;
extern int TimeTask_run;

#endif

