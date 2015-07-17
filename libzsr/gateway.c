#include <zsr/zsr.h>
#include <arpa/inet.h>

void get_gateway(gateway_t *gateway) {
    FILE *fp = NULL;
    char buf[LEN_256];

    if ((fp = fopen("/proc/net/route", "r")) == NULL) {
        print_log_l(LOG_ERROR, "/proc/net/route");
        return;
    }

    while ((fgets(buf, LEN_256, fp)) != NULL) {
        unsigned long hex_dest = 0;
        unsigned long hex_flag = 0;
        unsigned long hex_gateway = 0;

        sscanf(buf, "%*s%lx%lx%lx", &hex_dest, &hex_gateway, &hex_flag);

        if (hex_flag == 3 && hex_dest == 0 && hex_gateway != 0) {
            if (inet_ntop(AF_INET, &hex_gateway, buf, LEN_64) != NULL) {

                strncpy(gateway->addr, buf, LEN_64);
                break;
            }
        }
    }

    fclose(fp);
}
