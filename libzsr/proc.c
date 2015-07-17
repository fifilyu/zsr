#include <zsr/zsr.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/sysinfo.h>
#include <stdio.h>
#include <sys/stat.h>

void to_proc_file(const char *pid, const char *file, char *buf) {
    strcpy(buf, "/proc/");
    strcat(buf, pid);
    strcat(buf, "/");
    strcat(buf, file);
}

void read_pid_stat(const char *pid, proc_t *proc) {
    FILE *fp = NULL;
    char _buf[LEN_512];
    char *col = NULL;
    int i = 0;
    int end_stat = 0;
    char proc_file[LEN_256];
    uint64_t utime = 0;
    uint64_t stime = 0;

    to_proc_file(pid, "stat", proc_file);

    if ((fp = fopen(proc_file, "r")) == NULL) {
        print_log_l(LOG_ERROR, proc_file);
        return;
    }

    if (fgets(_buf, LEN_512, fp) == NULL) {
        fclose(fp);
        return;
    }

    fclose(fp);
    col = strtok(_buf, " ");

    do {
        switch (++i) {
            case 1:
                sscanf(col, "%lu", &proc->pid);
                break;
            case 2:
                sscanf(col, "(%[^)]s)", &proc->cmd);
                break;
            case 3:
                sscanf(col, "%c", &proc->state);
                break;
            case 4:
                sscanf(col, "%lu", &proc->ppid);
                break;
            case 14:
                sscanf(col, "%lu", &utime);
                break;
            case 15:
                sscanf(col, "%lu", &stime);
                break;
            case 22:
                sscanf(col, "%lu", &proc->start_time);
                end_stat = 1;
                break;
            default:
                break;
        }

        if (end_stat) break;

    } while ((col = strtok(NULL, " ")) != NULL);

    proc->cpu_time = utime + stime;
}

void read_pid_exe(const char *pid, proc_t *proc) {
    int ret = 0;
    char proc_file[LEN_256];

    to_proc_file(pid, "exe", proc_file);

    if ((ret = readlink(proc_file, proc->cmd_path, LEN_1024)) != -1)
        proc->cmd_path[ret] = '\0';
}

void read_pid_status(const char *pid, proc_t *proc) {
    FILE *fp = NULL;
    char _buf[LEN_512];
    uid_t uid = 0;
    struct passwd *pw = NULL;
    struct group *gp = NULL;
    char proc_file[LEN_256];

    to_proc_file(pid, "status", proc_file);

    if (!(fp = fopen(proc_file, "r")))
        return;

    while (fgets(_buf, LEN_128, fp) != NULL) {
        if (!strncmp(_buf, "Uid:", 4)) {
            sscanf(_buf + 4, "%u", &uid);
            continue;
        }

        if (!strncmp(_buf, "VmSize:", 7)) {
            sscanf(_buf + 7, "%lu", &proc->virt_mem_kb);
            continue;
        }

        if (!strncmp(_buf, "VmRSS:", 6)) {
            sscanf(_buf + 6, "%lu", &proc->res_mem_kb);
            break;
        }
    }

    fclose(fp);

    if ((pw = getpwuid(uid)) != NULL)
        strcpy(proc->user, pw->pw_name);

    if ((gp = getgrgid(pw->pw_gid)) != NULL)
        strcpy(proc->group, gp->gr_name);

}

void set_pid_mem_until(proc_t *proc) {
    struct sysinfo sys_info = {0};

    if (sysinfo(&sys_info) == 0) {
        proc->mem_util = (proc->res_mem_kb * 1024.0 * 100) / sys_info.totalram;
        proc->mem_util = round_with_prec(proc->mem_util, 2);
    }
}

void set_sys_cpu_time(cpu_stat_t *cpu, proc_t *proc) {
    proc->sys_cpu_time =
            cpu->guest + cpu->hardirq + cpu->idle + cpu->iowait + cpu->nice +
            cpu->number + cpu->softirq + cpu->steal + cpu->sys + cpu->user;
}

static void store_single_pid_info(const char *pid, proc_t *proc) {
    cpu_stat_t cpu = { 0 };

    read_cpu_stat(&cpu);

    read_pid_stat(pid, proc);
    read_pid_exe(pid, proc);
    read_pid_status(pid, proc);

    set_sys_cpu_time(&cpu, proc);
    set_pid_mem_until(proc);
}

void get_all_proc(list_head_t *head) {
    DIR *dir= NULL;
    struct dirent *ent;
    struct stat s;
    char file[LEN_256];
    proc_t *node = NULL;

    if ((dir = opendir("/proc")) == NULL)
        return;

    while ((ent = readdir(dir))) {
        if (ent->d_name[0] == '.')
            continue;

        strcpy(file, "/proc/");
        strcat(file, ent->d_name);

        if ((stat(file, &s) == 0) && (s.st_mode & S_IFDIR)) {
            if (is_digit(ent->d_name)) {
                node = malloc(sizeof(proc_t));
                memset(node, 0, sizeof(proc_t));

                store_single_pid_info(ent->d_name, node);

                list_add_tail(&(node->list), head);
            }
        }
    }

    closedir(dir);
}

// FIXME 太多循环，需要有优化
void compute_proc_cpu_stat(list_head_t *cur_list, list_head_t *prev_list) {
    proc_cpu_stat_array_t cur_cs_array;
    proc_cpu_stat_array_t prev_cs_array;
    proc_cpu_stat_array_t cs_array;
    int cur_num_proc = 0;
    int prev_num_proc = 0;
    int i = 0;
    int c = 0;
    int p = 0;

    proc_t *entry = NULL;

    list_for_each_entry(entry, cur_list, list) {
        ++cur_num_proc;
    }

    list_for_each_entry(entry, prev_list, list) {
        ++prev_num_proc;
    }

    cur_cs_array.pid = calloc(cur_num_proc, sizeof(uint64_t));
    cur_cs_array.cpu_time = calloc(cur_num_proc, sizeof(uint64_t));
    cur_cs_array.sys_cpu_time = calloc(cur_num_proc, sizeof(uint64_t));

    prev_cs_array.pid = calloc(prev_num_proc, sizeof(uint64_t));
    prev_cs_array.cpu_time = calloc(prev_num_proc, sizeof(uint64_t));
    prev_cs_array.sys_cpu_time = calloc(prev_num_proc, sizeof(uint64_t));

    // pid, 进程cpu时间差，系统cpu时间差
    cs_array.pid = calloc(cur_num_proc, sizeof(uint64_t));
    cs_array.cpu_time = calloc(cur_num_proc, sizeof(uint64_t));
    cs_array.sys_cpu_time = calloc(cur_num_proc, sizeof(uint64_t));

    list_for_each_entry(entry, cur_list, list) {
        cur_cs_array.pid[i] = entry->pid;
        cur_cs_array.cpu_time[i] = entry->cpu_time;
        cur_cs_array.sys_cpu_time[i] = entry->sys_cpu_time;
        ++i;
    }

    i = 0;

    list_for_each_entry(entry, prev_list, list) {
        prev_cs_array.pid[i] = entry->pid;
        prev_cs_array.cpu_time[i] = entry->cpu_time;
        prev_cs_array.sys_cpu_time[i] = entry->sys_cpu_time;
        ++i;
    }

    i = 0;
    for (c = 0; c < cur_num_proc; ++c) {
        for (p = 0; p < prev_num_proc; ++p) {
            if (cur_cs_array.pid[c] == prev_cs_array.pid[p]) {
                cs_array.pid[i] = cur_cs_array.pid[c];
                cs_array.cpu_time[i] =
                        cur_cs_array.cpu_time[c]
                            - prev_cs_array.cpu_time[c];
                cs_array.sys_cpu_time[i] =
                        cur_cs_array.sys_cpu_time[c]
                            - prev_cs_array.sys_cpu_time[c];

                i++;
                break;
            }
        }
    }

    for (i = 0; i < cur_num_proc; ++i) {
        list_for_each_entry(entry, cur_list, list) {
            if (entry->pid == cs_array.pid[i]) {
                entry->cpu_util =
                        (cs_array.cpu_time[i] * 100.0)
                        / cs_array.sys_cpu_time[i];
                break;
            }
        }
    }

    free(cs_array.pid);
    free(cs_array.cpu_time);
    free(cs_array.sys_cpu_time);

    free(prev_cs_array.pid);
    free(prev_cs_array.cpu_time);
    free(prev_cs_array.sys_cpu_time);

    free(cur_cs_array.pid);
    free(cur_cs_array.cpu_time);
    free(cur_cs_array.sys_cpu_time);
}


