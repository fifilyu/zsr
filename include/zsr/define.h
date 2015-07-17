#ifndef INCLUDE_ZSR_DEFINE_H_
#define INCLUDE_ZSR_DEFINE_H_

#include <stdint.h>
#include <stddef.h>

#define LEN_32      32
#define LEN_64      64
#define LEN_128     128
#define LEN_256     256
#define LEN_512     512
#define LEN_1024    1024
#define LEN_4096    4096
#define LEN_1M      1048576
#define LEN_10M     10485760

typedef uint64_t byte_t;

#define STRUCT_ARRAY_SIZE(var) (sizeof var / sizeof var[0])

typedef enum {
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL,
    LOG_OFF
} log_level_t;

typedef enum {
    LOS_FILE,
    LOS_STDOUT,
    LOS_STDERR
} log_output_stream_t;

typedef struct {
    log_level_t level;
    log_output_stream_t output_to;
    size_t max_byte;
    char path[LEN_512];
} log_config_t;

#endif /* INCLUDE_ZSR_DEFINE_H_ */
