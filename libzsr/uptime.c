#include <zsr/zsr.h>

static void set_uptime(const uint64_t total_sec, uptime_t *up) {
    const uint64_t minute_sec = 60;
    const uint64_t hour_sec = 3600;
    const uint64_t day_sec = 86400;
    const uint64_t month_sec = 2592000;
    const uint64_t year_sec = 31536000;

    uint64_t tmp_sec = 0;

    up->total_sec = total_sec;

    up->year = total_sec / year_sec;

    tmp_sec = total_sec % year_sec;
    up->month = tmp_sec / month_sec;

    tmp_sec = tmp_sec % month_sec;
    up->day = tmp_sec / day_sec;

    tmp_sec = tmp_sec % day_sec;
    up->hour = tmp_sec / hour_sec;

    tmp_sec = tmp_sec % hour_sec;
    up->minute = tmp_sec / minute_sec;
}

void read_uptime_stat(uptime_t *up) {
    FILE *fp = NULL;
    uint64_t total_sec = 0;

    if ((fp = fopen("/proc/uptime", "r")) == NULL) {
        print_log_l(LOG_ERROR, "/proc/uptime");
        return;
    }

    fscanf(fp, "%lu", &total_sec);
    fclose(fp);

    set_uptime(total_sec, up);
}
