#ifndef LCD_h
#define LCD_h                 

extern void LCD_cmd (char x);
extern void LCD_gen (char x); 
extern void LCD_read (char x);
extern void LCD_bars_init (void);  
extern void LCD_prt (char x);
extern void LCD_init (void);
extern void LCD_BL(int x);
extern void LCD_str(char *str);
extern void LCD_move(int m);
extern void LCD_goto(int x, int y);
extern void LCD_clr();
extern void BL_dimmer (char key, int LDR);

#endif

