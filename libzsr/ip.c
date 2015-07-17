#include <zsr/zsr.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netdb.h>

void get_ip_addr(list_head_t *head) {
    struct ifaddrs *ifaddr, *ifap;
    int family = 0;
    int host_ret = 0;
    int netmask_ret = 0;
    char host[INET6_ADDRSTRLEN];
    char netmask[LEN_64];
    interface_t *node = NULL;
    char *ipv6_tok = NULL;

    if (getifaddrs(&ifaddr) != 0)
        return;

    for (ifap = ifaddr; ifap != NULL; ifap = ifap->ifa_next) {
        if (ifap->ifa_addr == NULL)
            continue;

        family = ifap->ifa_addr->sa_family;

        if (family != AF_INET && family != AF_INET6)
            continue;

        if (node == NULL) {
            node = malloc(sizeof(interface_t));
            memset(node, 0, sizeof(interface_t));
        }

        strncpy(node->dev, ifap->ifa_name, LEN_64);
        node->ip_ver = (family == AF_INET) ? IPv4 : IPv6;
        socklen_t socklen =
                (family == AF_INET) ?
                        sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);

        int len =
                (family == AF_INET) ? INET_ADDRSTRLEN: INET6_ADDRSTRLEN;

        host_ret =
                getnameinfo(
                        ifap->ifa_addr, socklen, host,
                        len, NULL, 0, NI_NUMERICHOST);
        netmask_ret =
                getnameinfo(
                        ifap->ifa_netmask, socklen, netmask,
                        len, NULL, 0, NI_NUMERICHOST);

        if (host_ret != 0 || netmask_ret != 0) {
            free(node);
            continue;
        }

        if (family == AF_INET6) {
            ipv6_tok = strtok(host, "%");
            strncpy(node->ip_addr, ipv6_tok, len);
        } else
            strncpy(node->ip_addr, host, len);

        strncpy(node->netmask, netmask, len);

        list_add_tail(&(node->list), head);
        node = NULL;
    }

    freeifaddrs(ifaddr);
}
