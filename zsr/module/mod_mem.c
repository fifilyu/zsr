#include "../framework.h"

static const char *name = "mem";

static const char *opt = "--mem";

static const char *usage =
        "    --mem"
        "        Display amount of free and used memory\n"
        "                 (total, used, free)\n";

static mod_info_t info[] = {
    {"total"},
    {"used"},
    {"free"}
};

static void collect_record(module_t *mod) {
    mem_stat_t *entry = NULL;

    // 始终只保持链表中只有一个实例
    if (list_empty(&mod->record_list)) {
        entry = malloc(sizeof(mem_stat_t));
        memset(entry, 0, sizeof(mem_stat_t));

        list_add_tail(&entry->list, &mod->record_list);
    } else
        entry = list_last_entry(&mod->record_list, mem_stat_t, list);

    read_mem_stat(entry);
}

static size_t record_to_str(module_t *mod) {
    mem_stat_t *entry = NULL;
    size_t ret = 0;

    if (list_empty(&mod->record_list))
        return -1;

    entry = list_last_entry(&mod->record_list, mem_stat_t, list);

    ret = snprintf(
            mod->record,
            LEN_1M,
            "%s=%lu,%lu,%lu;\n",
            name,
            entry->total_kb,
            entry->used_kb,
            entry->free_kb);

    return ret;
}

static void free_record_list(list_head_t *head) {
    mem_stat_t *tmp = NULL;
    mem_stat_t *entry = NULL;

    FREE_LIST_HEAD(entry, tmp, head, list);
}

void mod_mem_register() {
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
