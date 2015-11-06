#include <zsr/zsr.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <linux/rtnetlink.h>

void get_network(list_head_t *head) {
    struct ifaddrs *ifaddr, *ifap;
    int family = 0;
    rtnl_link_t *node = NULL;

    if (getifaddrs(&ifaddr) != 0)
        return;

    for (ifap = ifaddr; ifap != NULL; ifap = ifap->ifa_next) {
        if (ifap->ifa_addr == NULL)
            continue;

        family = ifap->ifa_addr->sa_family;

        if (family != AF_PACKET || ifap->ifa_data == NULL)
            continue;

        if (node == NULL) {
            node = malloc(sizeof(rtnl_link_t));
            memset(node, 0, sizeof(rtnl_link_t));
        }

        strncpy(node->dev, ifap->ifa_name, LEN_64);

        struct rtnl_link_stats *stats = ifap->ifa_data;
        node->tx_packet = stats->tx_packets;
        node->rx_packet = stats->rx_packets;
        node->tx_byte = stats->tx_bytes;
        node->rx_byte = stats->rx_bytes;

        list_add_tail(&(node->list), head);
        node = NULL;
    }

    freeifaddrs(ifaddr);
}

void compute_network_stat(list_head_t *cur_list, list_head_t *pre_list) {

}
