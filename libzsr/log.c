#include <zsr/log.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// 104857600 byte == 100 MiB
log_config_t G_LOG_CONFIG =
    {LOG_INFO, LOS_STDERR, 104857600, "/var/log/zsr.log"};

size_t get_size_in_byte(const char *file_name) {
    struct stat buf;

    if (stat(file_name, &buf) == 0)
        return buf.st_size;

    return 0;
}

void open_log_file(FILE **file) {
    const size_t old_log_size_ = get_size_in_byte(G_LOG_CONFIG.path);

    if (old_log_size_ >= G_LOG_CONFIG.max_byte)
        // 覆盖
        *file = fopen(G_LOG_CONFIG.path, "wb");
    else
        // 追加
        *file = fopen(G_LOG_CONFIG.path, "ab+");

    if (*file == NULL) {
        fprintf(
                stderr,
                "cannot open \"%s\": ",
                G_LOG_CONFIG.path,
                strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void get_today(char *buf, const int len) {
    time_t t = time(NULL);
    strftime(buf, len, "%Y-%m-%d %H:%M:%S", localtime(&t));
}

void _print_log(
        log_level_t level,
        const char *file,
        int line,
        const char *fmt,
        ...) {
    char today[LEN_32] = { 0 };

    // 关闭日志
    if (level == LOG_OFF)
        return;

    // 关闭比用户设置更加详细的日志
    if (level < G_LOG_CONFIG.level)
        return;

    FILE *log_file = NULL;

    switch (G_LOG_CONFIG.output_to) {
        case LOS_STDOUT:
            log_file = stdout;
            break;
        case LOS_STDERR:
            log_file = stderr;
            break;
        case LOS_FILE:
            open_log_file(&log_file);
            break;
        default:
            log_file = stdout;
            break;
    }

    get_today(today, LEN_32);

    fprintf(log_file, "[%s] [%d] ", today, getpid());

    if (level <= LOG_TRACE)
        fprintf(log_file, "%s:%d ", file, line);

    va_list ap;
    va_start(ap, fmt);
    vfprintf(log_file, fmt, ap);
    va_end(ap);

    fprintf(log_file, "\n");
    fflush(log_file);

    if (G_LOG_CONFIG.output_to == LOS_FILE)
        fclose(log_file);
}
