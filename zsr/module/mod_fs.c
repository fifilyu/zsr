#include "../framework.h"

static const char *name = "fs";

static const char *opt = "--fs";

static const char *usage =
        "    --fs"
        "         Report file system disk space usage\n"
        "                 (mnt_fsname, mnt_dir, mnt_type, total, used, \n"
        "                 available, util, inodes, iused, ifree, iutil)\n";

static mod_info_t info[] = {
    {"mnt_fsname"},
    {"mnt_dir"},
    {"mnt_type"},
    {"total"},
    {"used"},
    {"available"},
    {"util"},
    {"inodes"},
    {"iused"},
    {"ifree"},
    {"iutil"}
};

static void collect_record(module_t *mod) {
    if (!list_empty(&mod->record_list))
        list_del_init(&mod->record_list);

    // 只会保留最后一次查询结果
    read_fs_stat(&mod->record_list);
}

static size_t record_to_str(module_t *mod) {
    fs_stat_t *entry = NULL;
    size_t ret = 0;

    if (list_empty(&mod->record_list))
        return -1;

    list_for_each_entry(entry, &mod->record_list, list)
        ret += snprintf(
                    mod->record + ret,
                    LEN_1M,
                    "%s=%s,%s,%s,%llu,%llu,%llu,%.2f,%llu,%llu,%llu,%.2f;\n",
                    name,
                    entry->mnt_fsname,
                    entry->mnt_dir,
                    entry->mnt_type,
                    entry->total,
                    entry->used,
                    entry->available,
                    entry->util,
                    entry->inodes,
                    entry->iused,
                    entry->ifree,
                    entry->iutil);

    return ret;
}

static void free_record_list(list_head_t *head) {
    fs_stat_t *tmp = NULL;
    fs_stat_t *entry = NULL;

    FREE_LIST_HEAD(entry, tmp, head, list);
}

void mod_fs_register() {
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
