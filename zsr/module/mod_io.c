// https://www.kernel.org/doc/Documentation/iostats.txt

#include "../framework.h"

static const char *name = "io";

static const char *opt = "--io";

static const char *usage =
        "    --io         Report input/output statistics for devices and "
        "partitions\n"
        "                 (dev, tps, util, rrqm/s, wrqm/s, r/s, w/s, rMB/s, "
        "wMB/s,\n"
        "                 avgrq-sz, avgqu-sz, await, r_await, w_await)\n";

static mod_info_t info[] = {
        {"dev"},
        {"tps"},
        {"util"},
        {"rrqm/s"},
        {"wrqm/s"},
        {"r/s"},
        {"w/s"},
        {"rMB/s"},
        {"wMB/s"},
        {"await"},
        {"avgqu-sz"},
        {"avgqu-sz"},
        {"r_await"},
        {"w_await"}
};

static void collect_record(module_t *mod) {
    list_list_t *cur_list = malloc(sizeof(list_list_t));
    list_list_t *prev_list = NULL;

    INIT_LIST_HEAD(&cur_list->head);

    read_io_stat(&cur_list->head);

    // 从第二次收集数据开始计算统计数据
    if (!list_empty(&mod->record_list)) {
        prev_list = list_last_entry(&mod->record_list, list_list_t, list);
        compute_io_stat(&cur_list->head, &prev_list->head);
    }

    list_add_tail(&cur_list->list, &mod->record_list);
}

static size_t record_to_str(module_t *mod) {
    list_list_t *cur_list = NULL;
    io_t *entry = NULL;
    size_t ret = 0;

    if (list_empty(&mod->record_list))
        return -1;

    cur_list = list_last_entry(&mod->record_list, list_list_t, list);

    list_for_each_entry(entry, &cur_list->head, list) {
        ret += snprintf(
                    mod->record + ret,
                    LEN_1M,
                    "%s=%s,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f;\n",
                    name,
                    entry->dev,
                    entry->tps,
                    entry->util / 10.0,
                    entry->rrqm_per_sec,
                    entry->wrqm_per_sec,
                    entry->r_per_sec,
                    entry->w_per_sec,
                    entry->r_speed_per_sec,
                    entry->w_speed_per_sec,
                    entry->avgrq_sz,
                    entry->avgqu_sz,
                    entry->await,
                    entry->r_await,
                    entry->w_await);
    }

    return ret;
}

static void free_record_list(list_head_t *head) {
    list_list_t *tmp_list = NULL;
    list_list_t *entry_list = NULL;
    io_t *tmp = NULL;
    io_t *entry = NULL;

    list_for_each_entry_safe(entry_list, tmp_list, head, list) {

        list_for_each_entry_safe(entry, tmp, &entry_list->head, list) {
            free(entry);
        }

        free(entry_list);
    }
}

void mod_io_register() {
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
