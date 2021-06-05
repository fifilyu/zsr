
#ifndef INCLUDE_ZSR_FRAMEWORK_H_
#define INCLUDE_ZSR_FRAMEWORK_H_

#include <zsr/zsr.h>

typedef struct mod_info {
    char col_name[LEN_128];
} mod_info_t;

typedef struct module {
    list_head_t list;

    char name[LEN_32];
    char opt_line[LEN_32];
    char usage[LEN_256];
    int enable;  // 0 disable，1 enable

    //FIXME 使用 char *record; size_t record_len; 代替，减少内存分配
    char record[LEN_1M];
    list_head_t record_list;

    int num_col;
    struct mod_info *info;
    void (*collect_record)(struct module *);
    void (*record_to_str)(struct module *);
    void (*free_record_list)(list_head_t *);
} module_t;

extern list_head_t MOD_LIST_HEAD;

void load_modules();

void INIT_MOD_LIST_HEAD();

void FREE_MOD_LIST_HEAD();

// @compute_stat 为 NULL，表示不需要计算
void register_mod_fileds(
        const char *name,
        struct mod_info *info,
        const char *opt,
        const char *usage,
        int num_col,
        void *collect_record,
        void *record_to_str,
        void *free_record_list);

#endif /* INCLUDE_ZSR_FRAMEWORK_H_ */
