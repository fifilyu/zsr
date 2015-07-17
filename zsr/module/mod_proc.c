#include "../framework.h"

static const char *name = "proc";

static const char *opt = "--proc";

static const char *usage =
        "    --proc"
        "       Display processes\n"
        "                 (pid, state, ppid, user, virt_mem_kb, res_mem_kb,\n"
        "                 mem_util, start_time, cpu_util, cmd, cmd_path)\n";

static mod_info_t info[] = {
    {"pid"},
    {"state"},
    {"ppid"},
    {"user"},
    {"virt_mem_kb"},
    {"res_mem_kb"},
    {"mem_util"},
    {"start_time"},
    {"cpu_util"},
    {"cmd"},
    {"cmd_path"}
};

static void collect_record(module_t *mod) {
    list_list_t *cur_list = malloc(sizeof(list_list_t));
    list_list_t *prev_list = NULL;

    INIT_LIST_HEAD(&cur_list->head);

    get_all_proc(&cur_list->head);

    // 从第二次收集数据开始计算统计数据
    if (!list_empty(&mod->record_list)) {
        prev_list = list_last_entry(&mod->record_list, list_list_t, list);
        compute_proc_cpu_stat(&cur_list->head, &prev_list->head);
    }

    list_add_tail(&cur_list->list, &mod->record_list);
}

static size_t record_to_str(module_t *mod) {
    list_list_t *cur_list = NULL;
    proc_t *entry = NULL;
    size_t ret = 0;

    if (list_empty(&mod->record_list))
        return -1;

    cur_list = list_last_entry(&mod->record_list, list_list_t, list);

    list_for_each_entry(entry, &cur_list->head, list)
        ret += sprintf(
                mod->record + ret,
                "%s=%lu,%c,%lu,%s,%s,%lu,%lu,%.2f,%lu,%.2f,%s;\n",
                name,
                entry->pid,
                entry->state,
                entry->ppid,
                entry->user,
                entry->group,
                entry->virt_mem_kb,
                entry->res_mem_kb,
                entry->mem_util,
                entry->start_time,
                entry->cpu_util,
                entry->cmd
                );


    return ret;
}

static void free_record_list(list_head_t *head) {
    list_list_t *tmp_list = NULL;
    list_list_t *entry_list = NULL;
    proc_t *tmp = NULL;
    proc_t *entry = NULL;

    list_for_each_entry_safe(entry_list, tmp_list, head, list) {

        list_for_each_entry_safe(entry, tmp, &entry_list->head, list) {
            free(entry);
        }

        free(entry_list);
    }
}

void mod_proc_register() {
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
