#include <petix/petix.h>
#include <petix/types.h>
#include <petix/string.h>
#include <petix/time.h>
#include <petix/io.h>

#define MIN(a, b) ((a)>(b)?(b):(a))
#define MAX(a, b) ((a)>(b)?(a):(b))

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

time_t mktime(const struct tm* t) {
    // 1970/1/1 00:00:00 UNIX Timestamp 0

    int leap_count = 0;
    if (t->tm_year >= 2000) {
        int years = t->tm_year - 2000;
        leap_count += years/400 * 97;
        years %= 400;
        leap_count += years/100 * 24;
        years %= 100;
        leap_count += (years-1)/4+1;
        // From 1970 to 2000 leap years: 7
        // 1972 1976 1980 1984 1988 1992 1996
        leap_count += 7;
    } else {
        for (int y=MIN(t->tm_year / 4 * 4 + (t->tm_year % 4 ? 4 : 0), 1972); y<MAX(t->tm_year, 1970); y+=4) {
            if (y % 400 != 0 && y % 100 == 0) continue;
            leap_count++;
        }
    }
    i64 day_count = (t->tm_year - 1970) * 365 + (t->tm_year < 1970 ? -leap_count : leap_count);
    
    int yday = t->tm_yday;
    if (yday < 0) {
        yday = nth_day_of_year(t->tm_year, t->tm_mon, t->tm_mday);
    }
    day_count += yday - 1;

    time_t sec_count = day_count * 86400 + t->tm_hour * 3600 + t->tm_min * 60 + t->tm_sec;
    return sec_count;
}

void fix_time(struct tm* t) {
    t->tm_yday = nth_day_of_year(t->tm_year, t->tm_mon, t->tm_mday);
    time_t mark = mktime(t);

    i64 uday = mark / 86400;
    i64 wkd = uday%7;
    if (wkd < 0) wkd += 7;
    wkd = (wkd+4)%7;
    t->tm_wday = wkd;
}

struct tm *gmtime_s(struct tm *t, const time_t *timer) {
    time_t time_val = *timer;

    // Timestamp of 2001/01/01 00:00:00 UTC+-00:00
    const time_t fix_delta = 978307200;

    time_t fixed_val = time_val - fix_delta;
    i64 days = fixed_val / 86400;
    
    int ct4 = days / 146097;    // 400 years = 146097 days, 400*365+97
    days %= 146097;
    int ct1 = days / 36524;     // 100 years = 36524 days, 100*365+24
    days %= 36524;
    int yr4 = days / 1461;      // 4 years = 1461 days, 4*365+1
    days %= 1461;
    if (yr4 < 0) {
        yr4--;
        days += 1461;
    }
    int yr1 = MIN(days / 365, 3);
    days -= yr1 * 365;

    int yday = days + 1;
    int year = ct4*400 + ct1*100 + yr4*4 + yr1 + 2001;
    int month = 0;
    bool lpy = leap_year(year);
    int mbd = 0;
    while (month < 12) {
        mbd = mth_begin_day[month];
        if (month >= 2 && lpy) {
            mbd++;
        }
        if (mbd <= days) {
            month++;
        } else break;
    }
    mbd = mth_begin_day[month-1];
    if (month > 2 && lpy) {
        mbd++;
    }
    days -= mbd;
    int day = days+1;

    t->tm_year = year;
    t->tm_mon = month;
    t->tm_mday = day;
    t->tm_yday = yday;

    // 1970/01/01 is Thursday!
    i64 uday = time_val / 86400;
    i64 wkd = (uday%7+4)%7;
    t->tm_wday = wkd;
    
    int secs = time_val % 86400;
    int hrs = secs / 3600;
    secs %= 3600;
    int min = secs / 60;
    secs %= 60;

    t->tm_hour = hrs;
    t->tm_min = min;
    t->tm_sec = secs;

    t->tm_isdst = 0;

    return t;
}

static char* strcpy_sized(char* dest, const char* src, size_t max_size) {
    for (const char* p = src; max_size > 0 && *p; p++, max_size--) {
        *dest++ = *p;
    }
    *dest = '\0';
    return dest;
}

static const char *strft_weekdays[7] = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday"
};

static const char *strft_months[13] = {
    "Empty",
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};

static char* strft_append_weekday(char *str, size_t maxsize, const struct tm *t) {
    return strcpy_sized(str, strft_weekdays[t->tm_wday], maxsize);
}

static char* strft_append_weekday_abbr(char *str, size_t maxsize, const struct tm *t) {
    return strcpy_sized(str, strft_weekdays[t->tm_wday], MIN(maxsize, 3));
}

static char* strft_append_month(char *str, size_t maxsize, const struct tm *t) {
    return strcpy_sized(str, strft_months[t->tm_mon], maxsize);
}

static char* strft_append_month_abbr(char *str, size_t maxsize, const struct tm *t) {
    return strcpy_sized(str, strft_months[t->tm_mon], MIN(maxsize, 3));
}

static char* strft_append_year(char *str, size_t maxsize, const struct tm *t) {
    char year[10];
    sprintf(year, "%d", t->tm_year);
    return strcpy_sized(str, year, maxsize);
}

static char* strft_append_year_abbr(char *str, size_t maxsize, const struct tm *t) {
    char year[10];
    sprintf(year, "%d", (t->tm_year) % 100);
    return strcpy_sized(str, year, MIN(maxsize, 2));
}

static char* strft_append_num(char *str, size_t maxsize, int val, int digit) {
    char buf[digit+1];
    sprintf(buf, "%0*d", digit, val);
    return strcpy_sized(str, buf, MIN(maxsize, digit));
}

static char* strft_append_hrs12(char *str, size_t maxsize, const struct tm *t) {
    int hr12 = t->tm_hour;
    if (hr12 != 0 && hr12 != 12) {
        hr12 %= 12;
    }
    return strft_append_num(str, maxsize, hr12, 2);
}

static char* strft_append_date(char *str, size_t maxsize, const struct tm *t) {
    char date[64];
    sprintf(date, "%02d/%02d/%02d", t->tm_mon, t->tm_mday, (t->tm_year)%100);
    return strcpy_sized(str, date, maxsize);
}

static char* strft_append_time(char *str, size_t maxsize, const struct tm *t) {
    char tm[64];
    sprintf(tm, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
    return strcpy_sized(str, tm, maxsize);
}

static char* strft_append_full_date_time(char *str, size_t maxsize, const struct tm *t) {
    char wname[4], mname[4];
    strcpy_sized(wname, strft_weekdays[t->tm_wday], 3);
    strcpy_sized(mname, strft_months[t->tm_mon], 3);

    char fulldate[128];
    sprintf(
        fulldate,
        "%3s %3s %02d %02d:%02d:%02d %d",
        wname,
        mname,
        t->tm_mday,
        t->tm_hour,
        t->tm_min,
        t->tm_sec,
        t->tm_year
    );
    return strcpy_sized(str, fulldate, maxsize);
}

static char* strft_append_week_count(char *str, size_t maxsize, const struct tm *t) {
    int days = t->tm_yday - 1;
    int week = t->tm_wday;
    int last_sunday = days - week;
    int weeks = last_sunday / 7 + 1;
    return strft_append_num(str, maxsize, weeks, 2);
}

static char* strft_append_week_count_w(char *str, size_t maxsize, const struct tm *t) {
    int days = t->tm_yday - 1;
    int week = t->tm_wday;
    int last_monday = days - week + 1;
    if (week == 0) last_monday -= 7;
    int weeks = last_monday / 7 + 1;
    return strft_append_num(str, maxsize, weeks, 2);
}

size_t strftime(char *str, size_t maxsize, const char *format, const struct tm *timeptr) {
    maxsize--;
    char *src_str = str;
    for (const char *fmtp = format; maxsize && *fmtp; fmtp++) {
        switch (*fmtp) {
        case '%':
            fmtp++;
            char c2 = *fmtp;
            char *str2 = str;
            switch (c2) {
            case 'a':
                str2 = strft_append_weekday_abbr(str, maxsize, timeptr);
                break;
            case 'A':
                str2 = strft_append_weekday(str, maxsize, timeptr);
                break;
            case 'b':
                str2 = strft_append_month_abbr(str, maxsize, timeptr);
                break;
            case 'B':
                str2 = strft_append_month(str, maxsize, timeptr);
                break;
            case 'c':
                str2 = strft_append_full_date_time(str, maxsize, timeptr);
                break;
            case 'd':
                str2 = strft_append_num(str, maxsize, timeptr->tm_mday, 2);
                break;
            case 'H':
                str2 = strft_append_num(str, maxsize, timeptr->tm_hour, 2);
                break;
            case 'l':
                str2 = strft_append_hrs12(str, maxsize, timeptr);
                break;
            case 'j':
                str2 = strft_append_num(str, maxsize, timeptr->tm_yday, 3);
                break;
            case 'm':
                str2 = strft_append_num(str, maxsize, timeptr->tm_mon, 2);
                break;
            case 'M':
                str2 = strft_append_num(str, maxsize, timeptr->tm_min, 2);
                break;
            case 'p':
                str2 = strcpy_sized(str, timeptr->tm_hour>=12?"PM":"AM", maxsize);
                break;
            case 'S':
                str2 = strft_append_num(str, maxsize, timeptr->tm_sec, 2);
                break;
            case 'U':
                str2 = strft_append_week_count(str, maxsize, timeptr);
                break;
            case 'w':
                str2 = strft_append_num(str, maxsize, timeptr->tm_wday, 1);
                break;
            case 'W':
                str2 = strft_append_week_count_w(str, maxsize, timeptr);
                break;
            case 'x':
                str2 = strft_append_date(str, maxsize, timeptr);
                break;
            case 'X':
                str2 = strft_append_time(str, maxsize, timeptr);
                break;
            case 'Y':
                str2 = strft_append_year(str, maxsize, timeptr);
                break;
            case 'y':
                str2 = strft_append_year_abbr(str, maxsize, timeptr);
                break;
            case 'Z':
                str2 = strcpy_sized(str, "UTC", maxsize);
            case '%':
                str2 = str+1;
                *str = '%';
                *(str+1) = '\0';
                break;
            }
            maxsize -= strlen(str);
            str = str2;
            break;
        default:
            *str++ = *fmtp;
            maxsize--;
            break;
        }
    }
    *str = '\0';
    return str - src_str;
}

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