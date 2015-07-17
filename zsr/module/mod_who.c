#include "../framework.h"

static const char *name = "who";

static const char *opt = "--who";

static const char *usage =
        "    --who"
        "        Show who is logged in\n"
        "                 (username, tty, from, login_time)\n";

static mod_info_t info[] = {
    {"username"},
    {"tty"},
    {"from"},
    {"login_time"}
};

static void collect_record(module_t *mod) {
    if (!list_empty(&mod->record_list))
        list_del_init(&mod->record_list);

    // 只会保留最后一次查询结果
    get_login_user(&mod->record_list);
}

static size_t record_to_str(module_t *mod) {
    login_user_t *entry = NULL;
    size_t ret = 0;

    if (list_empty(&mod->record_list))
        return -1;

    list_for_each_entry(entry, &mod->record_list, list)
    ret += snprintf(
                mod->record + ret,
                LEN_1M,
                "%s=%s,%s,%s,%ld;\n",
                name,
                entry->user,
                entry->tty,
                entry->from,
                entry->login_time);
    return ret;
}

static void free_record_list(list_head_t *head) {
    login_user_t *tmp = NULL;
    login_user_t *entry = NULL;

    FREE_LIST_HEAD(entry, tmp, head, list);
}


void mod_who_register() {
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
