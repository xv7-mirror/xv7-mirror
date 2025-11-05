/*
 * Routines for converting raw CMOS time to Unix
 * Copyright (c) 2025 Vladislav Prokopenko
 *
 */
#include "param.h"
#include "types.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "traps.h"
#include "mmu.h"
#include "x86.h"
#include "time.h"

/*
 * return 1 if leap year, 0 if not
 */
int leapyear(int year)
{
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

/*
 * days in every month (not leap year)
 */
const int mdays[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/*
 * convert from rtcdate struct (see lapic.c)
 * to Unix time.
 */
int unix_converttime(struct rtcdate* r)
{
    int i;
    int days = 0;

    for (i = 1970; i < r->year; i++)
        days += leapyear(i) ? 366 : 365;

    for (i = 1; i < r->month; i++) {
        days += mdays[i - 1];
        if (i == 2 && leapyear(r->year))
            days += 1;
    }

    days += r->day - 1;
    int seconds = days * 86400ULL;
    seconds += r->hour * 3600ULL;
    seconds += r->minute * 60ULL;
    seconds += r->second;

    return seconds;
}

struct rtcdate r;

/*
 * get current unix time
 */
int unix_uptime()
{
    cmostime(&r);
    return unix_converttime(&r);
}
