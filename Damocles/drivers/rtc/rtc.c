/* rtc.c */

#include "system/drivers/rtc.h"
#include "sysasm.h"

#define RTC_COMMAND_PORT 0x70
#define RTC_DATA_PORT 0x71
#define RTC_SECONDS 0
#define RTC_MINUTES	2
#define RTC_HOURS	4

#define RTC_REGISTER_B 0x0B
//Flags
#define BINARY_FORMAT (0x1 << 2)

static int read(int what);

void rtc_configure(){
	byte current;
	_out(RTC_COMMAND_PORT, RTC_REGISTER_B);
	current = _in(RTC_DATA_PORT);
	current |= BINARY_FORMAT;
	_out(RTC_DATA_PORT, current);
}

int rtc_getSeconds() {
	return read(RTC_SECONDS);
}

int rtc_getMinutes(){
	return read(RTC_MINUTES);
}

int rtc_getHours(){
	return read(RTC_HOURS);
}

void rtc_getTimeStr(char *str);

static int read(int what){
	int rta;
	_out(RTC_COMMAND_PORT, what);
	rta = _in(RTC_DATA_PORT);
	return rta;
}

