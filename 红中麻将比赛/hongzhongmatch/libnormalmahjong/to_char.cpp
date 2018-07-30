#include "to_char.h"
#include <stdio.h>

const char* to_char(int num)
{
	static char Number[64] = { 0 };
	sprintf(Number, "%d", num);
	return Number;
}
const char* to_char(long num)
{
	static char Number[64] = { 0 };
	sprintf(Number, "%ld", num);
	return Number;
}
const char* to_char(float num)
{
	static char Number[64] = { 0 };
	sprintf(Number, "%f", num);
	return Number;
}
