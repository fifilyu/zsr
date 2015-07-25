#include <zsr/zsr.h>

void read_load_stat(load_stat_t *load) {
    FILE *fp = NULL;
    char buf[LEN_128];

    if ((fp = fopen("/proc/loadavg", "r")) == NULL) {
        print_log_l(LOG_ERROR, "/proc/loadavg");
        return;
    }

    fgets(buf, LEN_128, fp);
    fclose(fp);

    sscanf(
            buf,
            "%f %f %f ",
            &load->one_min,
            &load->five_min,
            &load->fifteen_min
            );
}
