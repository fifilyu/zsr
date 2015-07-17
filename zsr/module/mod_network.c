#include "../framework.h"

static const char *name = "network";

static const char *opt = "--network";

static const char *usage =
        "    --network"
        "    Display bandwidth usage\n"
        "                 (dev, tx_packet, rx_packet, tx_byte, rx_byte)\n";

static mod_info_t info[] = {
    {"dev"},
    {"tx_packet"},
    {"rx_packet"},
    {"tx_byte"},
    {"rx_byte"}
};

static void collect_record(module_t *mod) {
    if (!list_empty(&mod->record_list))
        list_del_init(&mod->record_list);

    // 只会保留最后一次查询结果
    get_network(&mod->record_list);
}

static size_t record_to_str(module_t *mod) {
    rtnl_link_t *entry = NULL;
    size_t ret = 0;

    if (list_empty(&mod->record_list))
        return -1;

    list_for_each_entry(entry, &mod->record_list, list)
        ret += snprintf(
                    mod->record + ret,
                    LEN_1M,
                    "%s=%s,%lu,%lu,%lu,%lu;\n",
                    name,
                    entry->dev,
                    entry->tx_packet,
                    entry->rx_packet,
                    entry->tx_byte,
                    entry->rx_byte);

    return ret;
}

static void free_record_list(list_head_t *head) {
    rtnl_link_t *tmp = NULL;
    rtnl_link_t *entry = NULL;

    FREE_LIST_HEAD(entry, tmp, head, list);
}

void mod_network_register() {
    register_mod_fileds(
            name,
            info,
            opt,
            usage,
            STRUCT_ARRAY_SIZE(info),
            collect_record,
            record_to_str,
            free_record_list);
}
