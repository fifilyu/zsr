#ifndef INCLUDE_ZSR_ZSR_H_
#define INCLUDE_ZSR_ZSR_H_

#include <zsr/common.h>
#include <zsr/define.h>
#include <zsr/list.h>
#include <zsr/log.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint64_t user;
    uint64_t nice;
    uint64_t sys;
    uint64_t idle;
    uint64_t iowait;
    uint64_t steal;
    uint64_t hardirq;
    uint64_t softirq;
    uint64_t guest;
    uint64_t number;
} cpu_stat_t;

typedef struct {
    list_head_t list;

    double idle;  // 空闲率 = 100% - 使用率
    double util;  // 使用率 = 100% - 空闲率
    double wa;  // I/O 等待，使用率中包含 wa

    cpu_stat_t stat;
} cpu_t;

typedef struct {
    list_head_t list;

    char addr[LEN_64];
} dns_t;

typedef struct {
    list_head_t list;

    time_t time_in_sec;
} sys_time_t;

typedef struct {
    list_head_t list;

    char addr[LEN_64];
} gateway_t;


typedef struct {
    // Read system uptime from /proc/stat(first line, "cpu")
    uint64_t uptime;
    // Read system uptime (only for SMP machines) from /proc/uptime(first value)
    uint64_t uptime0;

    uint64_t rd_sectors;
    /* # of sectors written */
    uint64_t wr_sectors;
    /* # of read operations issued to the device */
    uint64_t rd_ios;
    /* # of read requests merged */
    uint64_t rd_merges;
    /* # of write operations issued to the device */
    uint64_t wr_ios;
    /* # of write requests merged */
    uint64_t wr_merges;
    /* Time of read requests in queue */
    uint64_t rd_ticks;
    /* Time of write requests in queue */
    uint64_t wr_ticks;
    /* # of I/Os in progress */
    uint64_t ios_pgr;
    /* # of ticks total (for this device) for I/O */
    uint64_t tot_ticks;
    /* # of ticks requests spent in queue */
    uint64_t rq_ticks;
} io_stat_t;

typedef struct {
    list_head_t list;

    char dev[LEN_64];
    double tps;
    byte_t read_byte;
    byte_t wrtn_byte;
    double rrqm_per_sec;
    double wrqm_per_sec;
    double r_per_sec;
    double w_per_sec;
    double r_speed_per_sec;
    double w_speed_per_sec;
    double avgrq_sz;
    double avgqu_sz;
    double await;
    double r_await;
    double w_await;
    double util;

    io_stat_t stat;
} io_t;

typedef enum {
    IPv4,
    IPv6
} ip_version_t;

typedef struct {
    list_head_t list;

    char dev[LEN_64];
    char ip_addr[LEN_64];
    ip_version_t ip_ver;
    char netmask[LEN_64];
} interface_t;

typedef struct {
    list_head_t list;

    char dev[LEN_64];
    uint64_t tx_packet;
    uint64_t rx_packet;
    uint64_t tx_byte;
    uint64_t rx_byte;
} rtnl_link_t;

typedef struct {
    list_head_t list;

    char user[LEN_32];
    char tty[LEN_32];
    char from[LEN_256];
    time_t login_time;
} login_user_t;

typedef struct {
    list_head_t list;

    float one_min;
    float five_min;
    float fifteen_min;
} load_stat_t;

typedef struct {
    list_head_t list;

    uint64_t total_kb;
    uint64_t used_kb;
    uint64_t free_kb;
} mem_stat_t;

typedef struct {
    list_head_t list;

    char mnt_fsname[LEN_1024];
    char mnt_dir[LEN_1024];
    char mnt_type[LEN_32];
    byte_t total;
    byte_t used;
    byte_t available;
    double util;
    uint64_t inodes;
    uint64_t iused;
    uint64_t ifree;
    double iutil;
} fs_stat_t;

typedef struct {
    list_head_t list;

    uint64_t pid;
    char state;
    uint64_t ppid;
    char user[LEN_128];
    char group[LEN_128];
    uint64_t virt_mem_kb;
    uint64_t res_mem_kb;
    double mem_util;
    uint64_t start_time;
    uint64_t sys_cpu_time;  // 系统总 cpu 时间
    // 进程 cpu 时间
    // 进程cpu使用率(cpu_util)=(当前cpu_time - 上次cpu_time)/(当前total_cpu_time - 上次total_cpu_time)
    // Amount of time that this process has been scheduled in user and kernel mode
    // utime + stime
    uint64_t cpu_time;
    double cpu_util;
    char cmd[LEN_256];
    char cmd_path[LEN_1024];
} proc_t;

// 列表作为一个整体@head存放在结构体中
typedef struct {
    list_head_t list;

    // @head 中包含更多的信息
    list_head_t head;
} list_list_t;

typedef struct {
    // 用于计算同一个进程的 CPU 使用率
    uint64_t *pid;
    uint64_t *sys_cpu_time;
    uint64_t *cpu_time;
} proc_cpu_stat_array_t;


typedef struct {
    list_head_t list;

    uint64_t total_kb;
    uint64_t used_kb;
    uint64_t free_kb;
} swap_stat_t;

typedef struct {
    list_head_t list;

    uint64_t total_sec;
    uint64_t year;
    uint64_t month;
    uint64_t day;
    uint64_t hour;
    uint64_t minute;
} uptime_t;

void read_cpu_stat(cpu_stat_t *cpu);
void get_dns(list_head_t *head);
void get_gateway(gateway_t *gateway);

void read_io_stat(list_head_t *head);
uint get_num_of_cpu();
uint64_t get_cpu_time_in_jiffies();
uint64_t read_uptime_in_jiffies();

void get_ip_addr(list_head_t *head);
void get_network(list_head_t *head);
void read_load_stat(load_stat_t *load);
void get_login_user(list_head_t *head);
void read_mem_stat(mem_stat_t *mem);
void read_fs_stat(list_head_t *head);
void get_all_proc(list_head_t *head);
void read_swap_stat(swap_stat_t *swap);
time_t get_sys_time();
void read_uptime_stat(uptime_t *up);

void compute_cpu_stat(cpu_t *cur_record, cpu_t *pre_record);
uint64_t sum_cpu_stat(cpu_stat_t *stat);

void compute_io_stat(list_head_t *cur_list, list_head_t *pre_list);
void compute_network_stat(list_head_t *cur_list, list_head_t *pre_list);
void compute_proc_cpu_stat(list_head_t *cur_list, list_head_t *prev_list);

#endif /* INCLUDE_ZSR_ZSR_H_ */
