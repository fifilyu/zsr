#include <zsr/zsr.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static bool is_local_host(const char *s) {
    return (strcmp(s, "0.0.0.0") == 0
            || strcmp(s, "127.0.0.1") == 0
            || strcmp(s, "::") == 0
            || strcmp(s, "::1") == 0);
}

void get_dns(list_head_t *head) {
    const int32_t ret_v = res_init();
    dns_t *node = NULL;
    uint32_t i = 0;

    if (ret_v != 0)
        return;

    for (i = 0; i < _res.nscount; i++) {
        char *ret = inet_ntoa(_res.nsaddr_list[i].sin_addr);

        if (ret != NULL && !is_local_host(ret)) {
            node = malloc(sizeof(dns_t));
            memset(node, 0, sizeof(dns_t));

            strncpy(node->addr, ret, LEN_64);
            list_add_tail(&(node->list), head);
            node = NULL;
        }
    }
}
