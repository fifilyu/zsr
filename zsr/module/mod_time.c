#include "../framework.h"

static const char *name = "time";

static const char *opt = "--time";

static const char *usage =
        "    --time"
        "       System time(seconds since 1970-01-01 00:00:00 UTC)\n"
        "                 (time)\n";

static mod_info_t info[] = {
    {"time"}
};

static void collect_record(module_t *mod) {
    sys_time_t *entry = NULL;

    // 始终只保持链表中只有一条数据
    if (list_empty(&mod->record_list)) {
        entry = malloc(sizeof(sys_time_t));
        memset(entry, 0, sizeof(sys_time_t));

        list_add_tail(&entry->list, &mod->record_list);
    } else
        entry = list_last_entry(&mod->record_list, sys_time_t, list);

    entry->time_in_sec = get_sys_time();
}

static size_t record_to_str(module_t *mod) {
    sys_time_t *entry = NULL;
    size_t ret = 0;

    if (list_empty(&mod->record_list))
        return -1;

    entry = list_last_entry(&mod->record_list, sys_time_t, list);

    ret = snprintf(
            mod->record,
            LEN_1M,
            "%s=%lu;\n",
            name,
            entry->time_in_sec);

    return ret;
}

static void free_record_list(list_head_t *head) {
    sys_time_t *tmp = NULL;
    sys_time_t *entry = NULL;

    FREE_LIST_HEAD(entry, tmp, head, list);
}

void mod_time_register() {
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
