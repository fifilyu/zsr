#include "../framework.h"

static const char *name = "cpu";

static const char *opt = "--cpu";

static const char *usage =
        "    --cpu"
        "        CPU share\n"
        "                 (idle, util, wa)\n";

static mod_info_t info[] = {
    {"idle"},  // 空闲率 = 100% - 使用率
    {"util"},  // 使用率 = 100% - 空闲率
    {"wa"}  // I/O 等待，使用率中包含 wa
};

static void collect_record(module_t *mod) {
    cpu_t *entry = malloc(sizeof(cpu_t));
    memset(entry, 0 , sizeof(cpu_t));
    cpu_t *prev_recodrd = NULL;

    read_cpu_stat(&entry->stat);

    // 从第二次收集数据开始计算统计数据
    if (!list_empty(&mod->record_list)) {
        prev_recodrd = list_last_entry(&mod->record_list, cpu_t, list);
        compute_cpu_stat(entry, prev_recodrd);
    }

    list_add_tail(&entry->list, &mod->record_list);
}

static size_t record_to_str(module_t *mod) {
    cpu_t *entry = NULL;
    size_t ret = 0;

    if (list_empty(&mod->record_list))
        return -1;

    entry = list_last_entry(&mod->record_list, cpu_t, list);

    ret = snprintf(
            mod->record,
            LEN_1M,
            "%s=%.2f,%.2f,%.2f;\n",
            name,
            entry->idle,
            entry->util,
            entry->wa);

    return ret;
}

static void free_record_list(list_head_t *head) {
    cpu_t *tmp = NULL;
    cpu_t *entry = NULL;

    FREE_LIST_HEAD(entry, tmp, head, list);
}

void mod_cpu_register() {
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
