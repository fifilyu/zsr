#include <zsr/zsr.h>
#include <mntent.h>
#include <sys/statfs.h>

void set_fs_stat(const char *mnt_dir, fs_stat_t *node) {
    struct statfs fs;

    if (statfs(mnt_dir, &fs) == 0) {
        node->total = fs.f_blocks * fs.f_bsize;
        // f_bfree 是所有未分配的空间，其中也包含文件系统日志等占用的空间
        // f_bfree = f_bavail + 日志等占用的空间 + ...
        node->used = (fs.f_blocks - fs.f_bfree) * fs.f_bsize;
        // f_bavail 是可以直接分配给用户的空间
        node->available = fs.f_bavail * fs.f_bsize;

        node->util = 1 - (node->available / (double)(node->total));
        node->util = round_with_prec(node->util, 2);

        node->inodes = fs.f_files;
        node->iused = fs.f_files - fs.f_ffree;
        node->ifree = fs.f_ffree;

        node->iutil = node->iused / (double)(node->inodes);
        node->iutil = round_with_prec(node->iutil, 2);
    }
}

void read_fs_stat(list_head_t *head) {
    FILE *fp = NULL;
    char line[LEN_128];
    struct mntent * mn = NULL;
    int pos = 0;
    fs_stat_t *node = NULL;

    if ((fp = setmntent("/etc/mtab", "r")) == NULL) {
        print_log_l(LOG_ERROR, "/etc/mtab");
        return;
    }

    while ((mn = getmntent(fp)) != NULL) {
        if (strncmp(mn->mnt_fsname, "/", 1) == 0) {
            node = malloc(sizeof(fs_stat_t));

            strncpy(node->mnt_fsname, mn->mnt_fsname, LEN_1024);
            strncpy(node->mnt_dir, mn->mnt_dir, LEN_1024);
            strncpy(node->mnt_type, mn->mnt_type, LEN_32);
            set_fs_stat(mn->mnt_dir, node);

            list_add_tail(&(node->list), head);
        }
    }

    endmntent(fp);
}
