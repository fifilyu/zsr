#include "framework.h"
#include <assert.h>

void mod_cpu_register();
void mod_dns_register();
void mod_fs_register();
void mod_gateway_register();
void mod_io_register();
void mod_ip_register();
void mod_load_register();
void mod_mem_register();
void mod_network_register();
void mod_proc_register();
void mod_swap_register();
void mod_time_register();
void mod_uptime_register();
void mod_who_register();

void load_modules() {
    mod_cpu_register();
    mod_dns_register();
    mod_fs_register();
    mod_gateway_register();
    mod_io_register();
    mod_ip_register();
    mod_load_register();
    mod_mem_register();
    mod_network_register();
    mod_proc_register();
    mod_swap_register();
    mod_time_register();
    mod_uptime_register();
    mod_who_register();
}

void INIT_MOD_LIST_HEAD() {
    INIT_LIST_HEAD(&MOD_LIST_HEAD);
}

void FREE_MOD_LIST_HEAD() {
    module_t *tmp = NULL;
    module_t *entry = NULL;

    list_for_each_entry_safe(entry, tmp, &MOD_LIST_HEAD, list) {
        list_del(&entry->list);
        free(entry);
    }
}

void register_mod_fileds(
        const char *name,
        struct mod_info *info,
        const char *opt,
        const char *usage,
        int num_col,
        void *collect_record,
        void *record_to_str,
        void *free_record_list) {
    module_t *mod = malloc(sizeof(module_t));
    memset(mod, 0, sizeof(module_t));

    sprintf(mod->name, "%s", name);
    sprintf(mod->opt_line, "%s", opt);
    sprintf(mod->usage, "%s", usage);

    mod->enable = 0;  // disable
    mod->info = info;
    mod->num_col = num_col;
    mod->collect_record = collect_record;
    mod->record_to_str = record_to_str;
    mod->free_record_list = free_record_list;

    INIT_LIST_HEAD(&mod->record_list);

    list_add_tail(&(mod->list), &MOD_LIST_HEAD);
}
