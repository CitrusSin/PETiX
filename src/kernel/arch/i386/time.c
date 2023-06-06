#include <petix/petix.h>
#include <petix/io.h>
#include <petix/time.h>
#include <petix/string.h>
#include <petix/arch/i386.h>

#define CMOS_OFFSET 0x70
#define CMOS_DATA 0x71

#define CMOS_SEC 0x00
#define CMOS_MIN 0x02
#define CMOS_HRS 0x04
#define CMOS_WEEKDAY 0x06
#define CMOS_MONTHDAY 0x07
#define CMOS_MONTH 0x08
#define CMOS_YEAR 0x09
#define CMOS_CENTURY 0x32

#define BCD_TO_BIN(val) ((((val)>>4)&0xf) * 10 + ((val) & 0xf))

static u8 cmos_read(u8 addr) {
    u8 data;
    OUT8(CMOS_OFFSET, addr);
    IN8(data, CMOS_DATA);
    return data;
}

static int mth_begin_day[12] = {
    0,
    31,
    31 + 28,
    31 + 28 + 31,
    31 + 28 + 31 + 30,
    31 + 28 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30,
    31 + 28 + 31 + 30 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30
};

static bool leap_year(int year) {
    return year % 4 == 0 && year % 100 != 0 || year % 400 == 0;
}

static int nth_day_of_year(int year, int mon, int day) {
    int day_count = mth_begin_day[mon-1];
    if (mon > 2 && leap_year(year)) day_count++;
    day_count += day;
    return day_count;
}

static void tm_now(struct tm *t) {
    u8 sec, min, hrs, wkd, mtd, m, y, c;
    
    ENTER_NON_INTERRUPT;

    do {
        sec = cmos_read(CMOS_SEC);
        min = cmos_read(CMOS_MIN);
        hrs = cmos_read(CMOS_HRS);
        wkd = cmos_read(CMOS_WEEKDAY);
        mtd = cmos_read(CMOS_MONTHDAY);
        m   = cmos_read(CMOS_MONTH);
        y   = cmos_read(CMOS_YEAR);
        c   = cmos_read(CMOS_CENTURY);
    } while (sec != cmos_read(CMOS_SEC));

    EXIT_NON_INTERRUPT;

    sec = BCD_TO_BIN(sec);
    min = BCD_TO_BIN(min);
    hrs = BCD_TO_BIN(hrs);
    wkd = BCD_TO_BIN(wkd);
    mtd = BCD_TO_BIN(mtd);
    m = BCD_TO_BIN(m);
    y = BCD_TO_BIN(y);
    c = BCD_TO_BIN(c);

    t->tm_sec = sec;
    t->tm_min = min;
    t->tm_hour = hrs;
    t->tm_mday = mtd;
    t->tm_mon = m;
    t->tm_year = c;
    t->tm_year *= 100;
    t->tm_year += y;
    t->tm_wday = wkd-1;

    t->tm_yday = nth_day_of_year(y, m, mtd);

    t->tm_isdst = 0;
}

time_t time(time_t *timer) {
    struct tm t;
    tm_now(&t);
    time_t tt = mktime(&t);
    if (timer != NULL) *timer = tt;
    return tt;
}

static time_t startup_time;

void time_init() {
    startup_time = time(NULL);  // Record startup time
}