#include "RTC.h"
#include "types.h"
#include "k_inout.h"

void k_read_RTC_time(uint8_t* hour, uint8_t* min, uint8_t* sec) {
    k_outb(RTC_CMOS_ADDR, RTC_ADDR_HOUR); // to cmos: please gimme hour
    uint8_t temp = k_inb(RTC_CMOS_DATA);
    *hour = RTC_BCD_TO_BIN(temp);

    k_outb(RTC_CMOS_ADDR, RTC_ADDR_MIN);
    temp = k_inb(RTC_CMOS_DATA);
    *min = RTC_BCD_TO_BIN(temp);

    k_outb(RTC_CMOS_ADDR, RTC_ADDR_SEC);
    temp = k_inb(RTC_CMOS_DATA);
    *sec = RTC_BCD_TO_BIN(temp);
}

void k_read_RTC_date(uint16_t* year, uint8_t* month, uint8_t* day, uint8_t* weekday) {
    k_outb(RTC_CMOS_ADDR, RTC_ADDR_YEAR);
    uint8_t temp = k_inb(RTC_CMOS_DATA);
    *year = RTC_BCD_TO_BIN(temp) + 2000;

    k_outb(RTC_CMOS_ADDR, RTC_ADDR_MONTH);
    temp = k_inb(RTC_CMOS_DATA);
    *month = RTC_BCD_TO_BIN(temp);

    k_outb(RTC_CMOS_ADDR, RTC_ADDR_DAY);
    temp = k_inb(RTC_CMOS_DATA);
    *day = RTC_BCD_TO_BIN(temp);

    k_outb(RTC_CMOS_ADDR, RTC_ADDR_WEEKDAY);
    temp = k_inb(RTC_CMOS_DATA);
    *weekday = RTC_BCD_TO_BIN(temp);

}

char* k_convert_weekday_to_str(uint8_t weekday) {
    static char* weekday_strs[8] = {"Error", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    return (weekday >= 8) ? weekday_strs[0] : weekday_strs[weekday];
}