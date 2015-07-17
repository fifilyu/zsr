#include <zsr/zsr.h>

void read_mem_stat(mem_stat_t *mem) {
    FILE *fp = NULL;
    char buf[LEN_128];

    if ((fp = fopen("/proc/meminfo", "r")) == NULL) {
        print_log_l(LOG_ERROR, "/proc/meminfo");
        return;
    }

    while (fgets(buf, LEN_128, fp)) {
        if (!strncmp(buf, "MemTotal:", 9)) {
            sscanf(buf + 9, "%lu", &mem->total_kb);
        } else if (!strncmp(buf, "MemFree:", 8)) {
            sscanf(buf + 8, "%lu", &mem->free_kb);
        } else
            break;
    }

    fclose(fp);

    mem->used_kb = mem->total_kb - mem->free_kb;
}
