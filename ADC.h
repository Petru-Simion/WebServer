#ifndef ADC_h              
#define ADC_h             

extern void celsius(void);
extern void fahrenheit(void);
extern int ADC_init (int channel);
extern int light (void);
extern void temp (void);
extern int ADC_init (int channel);
extern int getLDR ();
extern int getTempNatural();
extern int getTempFractional();

#endif
