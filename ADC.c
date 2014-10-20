#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <avr/io.h>

#include "LCD.h"
#include "ChangeTemp.h"
#include "BarGraph.h"

int adc_light;
int adc_temp, n, i;
int temp_array[10]= {};
long avg_temp ;
char fractional_string[10], natural_string[10];
int natural_temp = 0, frac_temp = 0;
char LDR_string[10];

/****************************/
// Init ADC
// Input: 0 (Temp) or 1 (LDR)
// Output: ADC value
/****************************/

int ADC_init (int channel)
{ 
   // Initialize ADMUX to select input and reference voltage
   switch (channel)
   {      
      case 0:
      ADMUX &= ~(0b00000001);    
      break;
      
      case 1:
      ADMUX |= 0b00000001;
      break;
   }
   
   // Initialize the control and status register 
   ADCSRA = 0b10010111;
   
   // Start the conversion     
   ADCSRA |=0b01000000;      
     
   while ((ADCSRA & 0x10) == 0);
   return (ADC);
}

/*********************************/
// Read average temperature value
/*********************************/

void temp (void)
{  
   unsigned long result = 0;
   int averages = 10;

   ADC_init(0); // PORTA, PIN 0
   adc_temp = ADC;
  
   // If array has been filled, reset counter:
   if (i == averages)          
   {           
      i = 0;
   }

   // Fill array with ADC measurements:
   else 
   {
   temp_array[i] = adc_temp; 
   i++;
   }  
   
   // Sums ADC measurements in array  
   for (n = 0; n < averages; n++)
   {
      result = result + temp_array[n];
   }
  
   // Check if array is full before dividing        
   if (i >= 10)
   {
   avg_temp = result/averages;           
   temp_master = true;
   }
}
    
/***********************/
// Celsius temperature
/***********************/

void celsius () 
{   
   int cel;

   cel = avg_temp;

   // 4.8875mV equals one step on ADC, 
   // 400mV equals 0 Celsius, 19.5mV equals 1 Celsius
   cel = ((((cel * 0.48875) - 0.400) / 0.0195) / 2); 

   // Calculate and print the natural part of the number
   natural_temp = cel/100; 
   itoa(natural_temp, natural_string,10);
   LCD_goto(13,1);
   LCD_str(natural_string);

   // Calculate and print fractional part of the number
   frac_temp= cel%100; 
   itoa (frac_temp, fractional_string,10); 
   LCD_goto (16,1);
   LCD_str(fractional_string);

   LCD_goto(19,1);
   LCD_str("C");
}

/**************************/
// Fahrenheit temperature
/**************************/

void fahrenheit () 
{
   int fahr;

   fahr = avg_temp;

   // Calcute celsius value:
   fahr = ((((fahr * 0.48875) - 0.400) / 0.0195) / 2);

   // Convert to fahrenheit:              
   fahr = fahr * 1.8 + 3200;                    

   // Calculate and print natural part:
   natural_temp = fahr/100;
   itoa(natural_temp, natural_string,10);    
   LCD_goto(13,1);
   LCD_str(natural_string);

   // Calculate and print fractional part:
   frac_temp= fahr%100;    
   itoa (frac_temp, fractional_string,10); 
   LCD_goto (16,1);
   LCD_str(fractional_string);

   LCD_goto(19,1);
   LCD_str("F");
}

/*************************/
// Light Sensor
// Output: Light intensity
/*************************/

int light (void)
{ 
  // char LDR_string[10];  
   char lead_1[2] = "0";
   char lead_2[3] = "00";
   char lead_3[4] = "000";

   ADC_init(1);

   // Invert ADC value and convert to string:
   adc_light = (1023-ADC);
   itoa(adc_light, LDR_string, 10);

   // Add leading zeros to string if necessary 
   // and print to LCD:
   if (strlen(LDR_string) == 1)
   {
      LCD_goto (5,2);
      LCD_str(strcat(lead_3, LDR_string));
   }

   if (strlen(LDR_string) == 2)
   {
      LCD_goto (5,2);
      LCD_str(strcat(lead_2, LDR_string));
   }

   if (strlen(LDR_string) == 3)
   {
      LCD_goto (5,2);
      LCD_str(strcat(lead_1, LDR_string));
   }

   if (strlen(LDR_string) == 4)
   {
      LCD_goto (5,2);
      LCD_str(LDR_string);
   }

   return adc_light;
}

int getLDR ()
{	
	return adc_light;
}

int getTempNatural()
{
	return natural_temp;
}

int getTempFractional()
{
	return frac_temp;
}
