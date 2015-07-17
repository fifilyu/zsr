#include "../framework.h"

static const char *name = "gateway";

static const char *opt = "--gateway";

static const char *usage =
        "    --gateway"
        "    Gateway\n"
        "                 (gateway)\n";

static mod_info_t info[] = {
    {"gateway"}
};

static void collect_record(module_t *mod) {
    gateway_t *entry = NULL;

    // 始终只保持链表中只有一个实例
    if (list_empty(&mod->record_list)) {
        entry = malloc(sizeof(gateway_t));
        memset(entry, 0, sizeof(gateway_t));

        list_add_tail(&entry->list, &mod->record_list);
    } else
        entry = list_last_entry(&mod->record_list, gateway_t, list);

    get_gateway(entry);
}

static size_t record_to_str(module_t *mod) {
    gateway_t *entry = NULL;
    size_t ret = 0;

    if (list_empty(&mod->record_list))
        return -1;

    entry = list_last_entry(&mod->record_list, gateway_t, list);
    ret = sprintf(
            mod->record,
            "%s=%s;\n",
            name,
            entry->addr);

    return ret;
}

static void free_record_list(list_head_t *head) {
    gateway_t *tmp = NULL;
    gateway_t *entry = NULL;

    FREE_LIST_HEAD(entry, tmp, head, list);
}

void mod_gateway_register() {
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
