#include <avr/io.h>
#include <stdio.h>

char s[40];

char* intToString(int x)
{
	sprintf(s, "%d", x);
	return s;
}
