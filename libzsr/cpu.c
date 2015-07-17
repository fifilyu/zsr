#include <zsr/zsr.h>

void read_cpu_stat(cpu_stat_t *cpu) {
    FILE *fp = NULL;
    char buf[LEN_128];

    if ((fp = fopen("/proc/stat", "r")) == NULL) {
        print_log_l(LOG_ERROR, "/proc/stat");
        return;
    }

    while (fgets(buf, LEN_128, fp)) {
        if (!strncmp(buf, "cpu ", 4)) {
            sscanf(
                    buf + 4,
                    "%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
                    &cpu->user,
                    &cpu->nice,
                    &cpu->sys,
                    &cpu->idle,
                    &cpu->iowait,
                    &cpu->steal,
                    &cpu->hardirq,
                    &cpu->softirq,
                    &cpu->guest,
                    &cpu->number);

            break;
        }
    }

    fclose(fp);
}

uint64_t sum_cpu_stat(cpu_stat_t *stat) {
    // Don't add cpu_guest/cpu_number because
    // cpu_user/cpu_nice already include them.
    const uint64_t total =
            stat->hardirq + stat->idle + stat->iowait + stat->nice +
            stat->softirq + stat->steal + stat->sys + stat->user;

    return total;
}

void compute_cpu_stat(cpu_t *cur_record, cpu_t *pre_record) {
    uint64_t cur_total = 0;
    uint64_t pre_total = 0;
    uint64_t total = 0;
    cpu_stat_t *cs = NULL;
    cpu_stat_t *ps = NULL;

    // $pre_record: first time
    // $cur_record: exception
    if (cur_record == NULL || pre_record == NULL)
        return;

    cs = &cur_record->stat;
    ps = &pre_record->stat;

    cur_total = sum_cpu_stat(cs);
    pre_total = sum_cpu_stat(ps);
    total = cur_total - pre_total;

    // first time
    if (pre_total == 0)
        return;

    // no tick changes, or tick overflows
    if (total <= 0)
        return;

    cur_record->idle =
            ((cs->idle - ps->idle) + (cs->steal - ps->steal)) * 100.0 / total;
    cur_record->util = 100.0 - cur_record->idle;
    cur_record->wa = (cs->iowait - ps->iowait) * 100.0 / total;
}

/*
 ***************************************************************************
 * Count number of processors in /proc/stat.
 *
 * RETURNS:
 * Number of processors. The returned value is greater than or equal to the
 * number of online processors.
 * A value of 0 means one processor and non SMP kernel.
 * A value of N (!=0) means N processor(s) (0 .. N-1) with SMP kernel.
 ***************************************************************************
 */
uint get_num_of_cpu(void)
{
    FILE *fp;
    char line[16];
    int cpu_num, num_of_cpu = -1;

    if ((fp = fopen("/proc/stat", "r")) == NULL) {
        print_log(LOG_FATAL, "Cannot open /proc/stat: %s\n", strerror(errno));
        exit(1);
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strncmp(line, "cpu ", 4) && !strncmp(line, "cpu", 3)) {
            sscanf(line + 3, "%d", &cpu_num);

            if (cpu_num > num_of_cpu) {
                num_of_cpu = cpu_num;
            }
        }
    }

    fclose(fp);

    return (uint)(num_of_cpu + 1);
}
