#include <time.h>
#include <stdint.h>

#include <windows.h>

#include "hal_time.h"

uint64_t
Hal_getTimeInMs(void)
{
	FILETIME ft;
	uint64_t now;

	static const uint64_t DIFF_TO_UNIXTIME = 11644473600000LL;

	GetSystemTimeAsFileTime(&ft);
	now = (LONGLONG)ft.dwLowDateTime + ((LONGLONG)(ft.dwHighDateTime) << 32LL);

	return (now / 10000LL) - DIFF_TO_UNIXTIME;
}

uint64_t
Hal_getCpuRunInMs(void)
{
	uint64_t now;
	long now_tick = 0;
	//CLOCKS_PER_SEC clock tick 1000

	now_tick = clock();//GetTickCount()
	if(now_tick < 0)//
	{
		now = Hal_getTimeInMs();
	}
	else
	{
		now = (uint64_t)((now_tick*1000)/CLOCKS_PER_SEC);
	}
	return now;//Hal_getTimeInMs();
}
