#include "../framework.h"

static const char *name = "uptime";

static const char *opt = "--uptime";

static const char *usage =
        "    --uptime"
        "     Tell how long the system has been running\n"
        "                 (total_sec, year, month, day, hour, minute)\n";

static mod_info_t info[] = {
    {"total_sec"},
    {"year"},
    {"month"},
    {"day"},
    {"hour"},
    {"minute"}
};

static void collect_record(module_t *mod) {
    uptime_t *entry = NULL;

    // 始终只保持链表中只有一条数据
    if (list_empty(&mod->record_list)) {
        entry = malloc(sizeof(uptime_t));
        memset(entry, 0, sizeof(uptime_t));

        list_add_tail(&entry->list, &mod->record_list);
    } else
        entry = list_last_entry(&mod->record_list, uptime_t, list);

    read_uptime_stat(entry);
}

static size_t record_to_str(module_t *mod) {
    uptime_t *entry = NULL;
    size_t ret = 0;

    if (list_empty(&mod->record_list))
        return -1;

    entry = list_last_entry(&mod->record_list, uptime_t, list);

    ret = snprintf(
            mod->record,
            LEN_1M,
            "%s=%lu,%lu,%lu,%lu,%lu,%lu;\n",
            name,
            entry->total_sec,
            entry->year,
            entry->month,
            entry->day,
            entry->hour,
            entry->minute);

    return ret;
}

static void free_record_list(list_head_t *head) {
    uptime_t *tmp = NULL;
    uptime_t *entry = NULL;

    FREE_LIST_HEAD(entry, tmp, head, list);
}

void mod_uptime_register() {
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
