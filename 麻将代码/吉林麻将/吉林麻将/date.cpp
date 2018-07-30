#include "date.h"
#include "time.h"

int GetWeekDay()
{
	time_t t;
	time(&t);
	tm* cur = localtime(&t);	
	return cur->tm_wday;
}