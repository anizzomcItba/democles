/*
 * rtc.h
 *
 *  Created on: Oct 26, 2011
 *      Author: anizzomc
 */

#ifndef RTC_H_
#define RTC_H_

void rtc_configure();

int rtc_getSeconds();

int rtc_getMinutes();

int rtc_getHours();

void rtc_getTimeStr(char *str);

#endif /* RTC_H_ */
