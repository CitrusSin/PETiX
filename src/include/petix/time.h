#ifndef PETIX_TIME_H
#define PETIX_TIME_H

#include "./types.h"

typedef u64 clock_t;
typedef i64 time_t;

struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

time_t mktime(const struct tm* t);
time_t time(time_t *timer);
struct tm *gmtime_s(struct tm *t, const time_t *timer);
size_t strftime(char *str, size_t maxsize, const char *format, const struct tm *timeptr);

void fix_time(struct tm* t);

#endif