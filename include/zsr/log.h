/*
 * zlog.h
 *
 *  Created on: 2014.9.30
 *      Author: Fifi Lyu
 *        Desc: 无任何非系统库依赖，功能简单的日志系统
 */

#ifndef INCLUDE_ZSR_LOG_H_
#define INCLUDE_ZSR_LOG_H_

#include <zsr/define.h>

extern log_config_t G_LOG_CONFIG;

void _print_log(
        log_level_t level, const char *file, int line, const char *fmt, ...);

#define print_log(level, ...) \
        _print_log(level, __FILE__, __LINE__, __VA_ARGS__)

#endif /* INCLUDE_ZSR_LOG_H_ */
