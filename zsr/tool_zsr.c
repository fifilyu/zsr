#include "framework.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>

module_t *mod = NULL;

void collect_record();
void free_record_list();
void enable_module(const char *mod_opt);
void enable_all_module();
void list_enable_module();
void list_enable_module();
void check_enable_module();
void usage();

struct option longopts[] = {
    {"all",  no_argument, NULL, 'a'},
    {"count",  required_argument, NULL, 'c'},
    {"delay",  required_argument, NULL, 'd'},
    {"list", no_argument, NULL, 'l'},
    {"load", no_argument, NULL, 'L'},
    {"help", no_argument, NULL, 'h'},
    {0,       0,             0,      0},
};

void collect_record() {
    list_for_each_entry(mod, &MOD_LIST_HEAD, list)
        mod->collect_record(mod);
}

void free_record_list() {
    list_for_each_entry(mod, &MOD_LIST_HEAD, list)
        mod->free_record_list(&mod->record_list);
}

void enable_module(const char *mod_opt) {
    list_for_each_entry(mod, &MOD_LIST_HEAD, list) {
        if (strcmp(mod->opt_line, mod_opt) == 0) {
            mod->enable = 1;
            return;
        }
    }

    fprintf(stderr, "Invalid module option: %s\n", mod_opt);
    exit(0);
}

void enable_all_module() {
    list_for_each_entry(mod, &MOD_LIST_HEAD, list)
        mod->enable = 1;
}

void list_enable_module() {
    fprintf(stderr, "\nModules Enabled:\n");

    list_for_each_entry(mod, &MOD_LIST_HEAD, list) {
        if (mod->enable)
            fprintf(stderr, "    %s\n", mod->opt_line);
    }

    fprintf(stderr, "\n");
}

void list_load_module() {
    fprintf(stderr, "\nModules Loaded:\n");

    list_for_each_entry(mod, &MOD_LIST_HEAD, list)
        fprintf(stderr, "%s\n", mod->usage);

    fprintf(stderr, "\n");
}

void check_enable_module() {
    int i = 0;

    list_for_each_entry(mod, &MOD_LIST_HEAD, list) {
        if (mod->enable)
            ++i;
    }

    if (i == 0) {
        fprintf(stderr, "No module is enabled.\n");
        exit(0);
    }
}

void usage() {
    fprintf(stderr,
            "\n"
            "Usage: zsr [options]\n"
            "\n"
            "Options:\n"
            "    -a, --all       enabled all modules\n"
            "    -c, --count     "
            "the value of count determines the number of reports generated\n"
            "    -d, --delay     delay-time interval\n"
            "    -l, --list      list enabled modules\n"
            "    -h, --help      help\n");

    list_load_module();
    list_enable_module();

    exit(0);
}

void print_mod_record() {
    list_for_each_entry(mod, &MOD_LIST_HEAD, list) {
        if (!mod->enable)
            continue;

        mod->record_to_str(mod);
        printf("%s", mod->record);
    }
}

int main(int argc, char **argv) {
    int i = 0;
    int opt = 0;
    int _optind = 0;
    module_t *mod = NULL;
    int opt_count = -1;
    int opt_interval = 2;
    int is_loop = 0;

    INIT_MOD_LIST_HEAD();

    load_modules();

    while (-1 !=
            (opt = getopt_long(argc, argv, "::alLh::c:d:", longopts, NULL))
            ) {
        switch (opt) {
            case 'a':
                enable_all_module();
                break;
            case 'c':
                opt_count = atoi(optarg);
                break;
            case 'd':
                opt_interval = atoi(optarg) > 0 ? atoi(optarg) : 2;
                break;
            case 'l':
                list_enable_module();
                exit(0);
                break;
            case 'L':
                list_load_module();
                exit(0);
                break;
            case 'h':
                usage();
                break;
            case ':':
                printf("must have parameter\n");
                usage();
                break;
            case '?':
                _optind = optind - 1;

                if (argv[_optind] && strstr(argv[_optind], "--"))
                    enable_module(argv[_optind]);
                else
                    usage();
                break;
            default:
                usage();
                break;
        }
    }

    is_loop = opt_count <= 0 ? 1 : 0;
    check_enable_module();

    while(1) {
        if (!is_loop && opt_count-- <= 0) {
            // 限制采集次数
            break;
        } else {
            // 循环运行，不限制采集次数
        }

        if (++i != 1)
            sleep(opt_interval);

        collect_record();
        print_mod_record();
    }

    free_record_list();
    FREE_MOD_LIST_HEAD();

    return EXIT_SUCCESS;
}
