#include <zsr/zsr.h>

void read_swap_stat(swap_stat_t *swap) {
    FILE *fp = NULL;
    char line[LEN_128];

    if ((fp = fopen("/proc/meminfo", "r")) == NULL) {
        print_log_l(LOG_ERROR, "/proc/meminfo");
        return;
    }

    while (fgets(line, LEN_128, fp)) {
        if (!strncmp(line, "SwapTotal:", 10)) {
            sscanf(line + 10, "%lu", &swap->total_kb);
            continue;
        }

        if (!strncmp(line, "SwapFree:", 9)) {
            sscanf(line + 9, "%lu", &swap->free_kb);
            break;
        }
    }

    fclose(fp);

    swap->used_kb = swap->total_kb - swap->free_kb;
}
