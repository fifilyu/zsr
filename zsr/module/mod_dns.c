#include "../framework.h"

static const char *name = "dns";

static const char *opt = "--dns";

static const char *usage =
        "    --dns"
        "        Domain Name System\n"
        "                 (dns)\n";

static mod_info_t info[] = {
    {"dns"}
};

static void collect_record(module_t *mod) {
    if (!list_empty(&mod->record_list))
        list_del_init(&mod->record_list);

    // 只会保留最后一次查询结果
    get_dns(&mod->record_list);
}

static size_t record_to_str(module_t *mod) {
    dns_t *entry = NULL;
    size_t ret = 0;

    if (list_empty(&mod->record_list))
        return -1;

    list_for_each_entry(entry, &mod->record_list, list)
        ret += sprintf(
                mod->record + ret,
                "%s=%s;\n",
                name,
                entry->addr);

    return ret;
}

static void free_record_list(list_head_t *head) {
    dns_t *tmp = NULL;
    dns_t *entry = NULL;

    FREE_LIST_HEAD(entry, tmp, head, list);
}

void mod_dns_register() {
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
