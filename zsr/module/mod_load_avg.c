#include "../framework.h"

static const char *name = "load_avg";

static const char *opt = "--load_avg";

static const char *usage =
        "    --load_avg"
        "   Load average\n"
        "                 (one_min, five_min, fifteen_min)\n";

static mod_info_t info[] = {
    {"one_min"},
    {"five_min"},
    {"fifteen_min"}
};

static void collect_record(module_t *mod) {
    load_stat_t *entry = NULL;

    // 始终只保持链表中只有一个实例
    if (list_empty(&mod->record_list)) {
        entry = malloc(sizeof(load_stat_t));
        memset(entry, 0, sizeof(load_stat_t));

        list_add_tail(&entry->list, &mod->record_list);
    } else
        entry = list_last_entry(&mod->record_list, load_stat_t, list);

    read_load_stat(entry);
}

static size_t record_to_str(module_t *mod) {
    load_stat_t *entry = NULL;
    size_t ret = 0;

    if (list_empty(&mod->record_list))
        return -1;

    entry = list_last_entry(&mod->record_list, load_stat_t, list);

    ret = snprintf(
            mod->record,
            LEN_1M,
            "%s=%.2f,%.2f,%.2f;\n",
            name,
            entry->one_min,
            entry->five_min,
            entry->fifteen_min);

    return ret;
}

static void free_record_list(list_head_t *head) {
    load_stat_t *tmp = NULL;
    load_stat_t *entry = NULL;

    FREE_LIST_HEAD(entry, tmp, head, list);
}

void mod_load_avg_register() {
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
