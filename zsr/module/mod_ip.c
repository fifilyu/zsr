#include "../framework.h"

static const char *name = "ip";

static const char *opt = "--ip";

static const char *usage =
        "    --ip"
        "         IP address\n"
        "                 (dev, ip, netmask, ip_version)\n";

static mod_info_t info[] = {
    {"dev"},
    {"ip"},
    {"netmask"},
    {"ip_version"}
};

static void collect_record(module_t *mod) {
    if (!list_empty(&mod->record_list))
        list_del_init(&mod->record_list);

    // 只会保留最后一次查询结果
    get_ip_addr(&mod->record_list);
}

static size_t record_to_str(module_t *mod) {
    interface_t *entry = NULL;
    size_t ret = 0;

    if (list_empty(&mod->record_list))
        return -1;

    list_for_each_entry(entry, &mod->record_list, list)
        ret += sprintf(
                    mod->record + ret,
                    "%s=%s,%s,%s,%d;\n",
                    name,
                    entry->dev,
                    entry->ip_addr,
                    entry->netmask,
                    entry->ip_ver);

    return ret;
}

static void free_record_list(list_head_t *head) {
    interface_t *tmp = NULL;
    interface_t *entry = NULL;

    FREE_LIST_HEAD(entry, tmp, head, list);
}

void mod_ip_register() {
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
