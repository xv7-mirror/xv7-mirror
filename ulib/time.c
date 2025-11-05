#include <stdio.h>
#include <time.h>

char* ctime(time_t t)
{
    static char buf[20]; /* YYYY-MM-DD HH:MM:SS + \0 */
    int sec, min, hour, day, month, year;
    static const int mdays[12]
        = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    sec = t % 60;
    t /= 60;
    min = t % 60;
    t /= 60;
    hour = t % 24;
    t /= 24;
    year = 1970;
    while (1) {
        int days = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
            ? 366
            : 365;
        if (t >= days) {
            t -= days;
            year++;
        } else
            break;
    }
    month = 0;
    for (;;) {
        int days = mdays[month];
        if (month == 1
            && (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)))
            days++;
        if (t >= days)
            t -= days, month++;
        else
            break;
    }

    day = t + 1;
    snprintf(buf, sizeof(buf), "%d-%d-%d %d:%d:%d", year, month + 1, day, hour,
        min, sec);

    return buf;
}
