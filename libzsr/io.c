#include <zsr/zsr.h>
#include <time.h>
#include <unistd.h>

static uint HZ = 100;
static uint NUM_OF_CPU = 1;

// Get number of clock ticks per second.
void get_HZ() {
    long ticks = sysconf(_SC_CLK_TCK);

    if (ticks == -1) {
        perror("sysconf");
        exit(1);
    }

    HZ = (uint)ticks;
}

void read_io_stat(list_head_t *head) {
    FILE *fp = NULL;
    char buf[LEN_256];
    struct mntent * mn = NULL;
    int pos = 0;
    io_t *node = NULL;
    int ret = 0;

    time_t sys_time = time(NULL);
    char dev[LEN_64];

    unsigned int major, minor;
    uint64_t ios_pgr, tot_ticks, rq_ticks, wr_ticks;
    uint64_t rd_ios, rd_merges_or_rd_sec, rd_ticks_or_wr_sec, wr_ios;
    uint64_t wr_merges, rd_sec_or_wr_ios, wr_sec;

    NUM_OF_CPU = get_num_of_cpu();
    get_HZ();

    if ((fp = fopen("/proc/diskstats", "r")) == NULL) {
        print_log_l(LOG_ERROR, "/proc/diskstats");
        return;
    }

    while(fgets(buf, LEN_256, fp) != NULL) {
        node = malloc(sizeof(io_t));
        memset(node, 0, sizeof(io_t));

        node->stat.uptime = get_cpu_time_in_jiffies();

        // SMP machine
        if (NUM_OF_CPU > 1)
            node->stat.uptime0 = read_uptime_in_jiffies();

        ret = sscanf(
                buf,
                "%u %u %s "
                "%lu %lu %lu %lu "
                "%lu %lu %lu %lu %lu %lu %lu",
                &major, &minor, &dev,
                &rd_ios, &rd_merges_or_rd_sec, &rd_sec_or_wr_ios, &rd_ticks_or_wr_sec,
                &wr_ios, &wr_merges, &wr_sec, &wr_ticks, &ios_pgr, &tot_ticks, &rq_ticks);

        if (ret != 14) {
            print_log(LOG_FATAL, "Unknown /proc/diskstats: Ignore it");
            return;
        }

        strncpy(node->dev, dev, LEN_64);
        node->stat.rd_ios     = rd_ios;
        node->stat.rd_merges  = rd_merges_or_rd_sec;
        node->stat.rd_sectors = rd_sec_or_wr_ios;
        node->stat.rd_ticks   = rd_ticks_or_wr_sec;
        node->stat.wr_ios     = wr_ios;
        node->stat.wr_merges  = wr_merges;
        node->stat.wr_sectors = wr_sec;
        node->stat.wr_ticks   = wr_ticks;
        node->stat.ios_pgr    = ios_pgr;
        node->stat.tot_ticks  = tot_ticks;
        node->stat.rq_ticks   = rq_ticks;

        list_add_tail(&node->list, head);
    }
}

#define COMPUTE_STAT(c, p, itv) ((double)(((c) - (p))) / (itv) * HZ)

void compute_io_stat(list_head_t *curr_list, list_head_t *prev_list) {
    uint64_t interval = 1;
    io_t *curr_entry = NULL;
    io_t *prev_entry = NULL;
    io_stat_t *cs = NULL;
    io_stat_t *ps = NULL;

    uint64_t cs_nr_ios = 0;
    uint64_t ps_nr_ios = 0;
    uint64_t change_nr_ios = 0;

    uint64_t change_rd_ticks = 0;
    uint64_t change_wr_ticks = 0;

    uint64_t change_rd_ios = 0;
    uint64_t change_wr_ios = 0;

    uint64_t change_rd_sectors = 0;
    uint64_t change_wr_sectors = 0;

    const byte_t SECTOR_IN_BYTE = 512;
    // KB
    //const int factor_ = 1024 / SECTOR_IN_BYTE;

    // MB
    const int factor_ = 1024 * 1024 / SECTOR_IN_BYTE;

    get_HZ();

    list_for_each_entry(curr_entry, curr_list, list) {
        list_for_each_entry(prev_entry, prev_list, list) {
            if (strcmp(curr_entry->dev, prev_entry->dev) != 0)
                continue;

            cs = &curr_entry->stat;
            ps = &prev_entry->stat;

            // SMP machine
            if (NUM_OF_CPU > 1)
                interval = cs->uptime0 - ps->uptime0;
            else
                interval = cs->uptime - ps->uptime;

            cs_nr_ios = cs->rd_ios + cs->wr_ios;
            ps_nr_ios = ps->rd_ios + ps->wr_ios;
            change_nr_ios = cs_nr_ios - ps_nr_ios;

            change_rd_ticks = cs->rd_ticks - ps->rd_ticks;
            change_wr_ticks = cs->wr_ticks - ps->wr_ticks;

            change_rd_ios = cs->rd_ios - ps->rd_ios;
            change_wr_ios = cs->wr_ios - ps->wr_ios;

            change_rd_sectors = cs->rd_sectors - ps->rd_sectors;
            change_wr_sectors = cs->wr_sectors - ps->wr_sectors;

            curr_entry->tps =
                    COMPUTE_STAT(
                            cs->rd_ios + cs->wr_ios,
                            ps->rd_ios + ps->wr_ios,
                            interval
                            );
            curr_entry->read_byte = change_rd_sectors / factor_;
            curr_entry->wrtn_byte = change_wr_sectors / factor_;

            curr_entry->rrqm_per_sec =
                    COMPUTE_STAT(cs->rd_merges, ps->rd_merges, interval);
            curr_entry->wrqm_per_sec =
                    COMPUTE_STAT(cs->wr_merges, ps->wr_merges, interval);
            curr_entry->r_per_sec =
                    COMPUTE_STAT(cs->rd_ios, ps->rd_ios, interval);
            curr_entry->w_per_sec =
                    COMPUTE_STAT(cs->wr_ios, ps->wr_ios, interval);
            curr_entry->r_speed_per_sec =
                    COMPUTE_STAT(cs->rd_sectors, ps->rd_sectors, interval)
                    / factor_;
            curr_entry->w_speed_per_sec =
                    COMPUTE_STAT(cs->wr_sectors, ps->wr_sectors, interval)
                    / factor_;

            curr_entry->avgrq_sz =
                    change_nr_ios ?
                            (change_rd_sectors + change_wr_sectors)
                            / ((double) change_nr_ios) : 0.0;

            curr_entry->avgqu_sz =
                    COMPUTE_STAT(cs->rq_ticks, ps->rq_ticks, interval) / 1000.0;

            curr_entry->await =
                    change_nr_ios ?
                            (change_rd_ticks + change_wr_ticks)
                            / ((double) change_nr_ios)
                            : 0.0;
            curr_entry->r_await =
                    change_rd_ios ? change_rd_ticks
                            / ((double) change_rd_ios) : 0.0;
            curr_entry->w_await =
                    change_wr_ios ? change_wr_ticks
                            / ((double) change_wr_ios) : 0.0;

            curr_entry->util =
                    COMPUTE_STAT(cs->tot_ticks, ps->tot_ticks, interval);

            break;
        }
    }
}

uint64_t get_cpu_time_in_jiffies() {
    cpu_stat_t stat = { 0 };

    read_cpu_stat(&stat);

    return sum_cpu_stat(&stat);
}

uint64_t read_uptime_in_jiffies() {
    uint64_t uptime = 0;
    FILE *fp;
    char line[LEN_128];
    uint64_t up_sec, up_cent;

    if ((fp = fopen("/proc/uptime", "r")) == NULL)
        return uptime;

    if (fgets(line, sizeof(line), fp) != NULL) {
        sscanf(line, "%lu.%lu", &up_sec, &up_cent);
        uptime = up_sec * HZ + up_cent * HZ / 100;
    }

    fclose(fp);
    return uptime;
}
