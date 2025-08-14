#pragma once

#include "types.h"

#define RTC_CMOS_ADDR 0x70
#define RTC_CMOS_DATA 0x71

#define RTC_ADDR_SEC 0x00
#define RTC_ADDR_MIN 0x02
#define RTC_ADDR_HOUR 0x04
#define RTC_ADDR_WEEKDAY 0x06
#define RTC_ADDR_DAY 0x07
#define RTC_ADDR_MONTH 0x08
#define RTC_ADDR_YEAR 0x09

#define RTC_BCD_TO_BIN(x) (  (((x) >> 4) * 10) + ( (x) & 0x0F )   )

void k_read_RTC_time(uint8_t* hour, uint8_t* min, uint8_t* sec);
void k_read_RTC_date(uint16_t* year, uint8_t* month, uint8_t* day, uint8_t* week);
char* k_convert_weekday_to_str(uint8_t weekday);