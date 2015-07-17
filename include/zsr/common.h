
#ifndef INCLUDE_ZSR_COMMON_H_
#define INCLUDE_ZSR_COMMON_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <zsr/define.h>

// 支持负数、小数
bool is_digit(const char *s);

/*
四舍五入浮点数
d 为原来的值
precision 为要保留的小数个数，也就是精度

返回值：类似
传入4.9999，返回5
传入4.9778，精度2，返回4.98
 */
double round_with_prec(const double d, const uint32_t precision);

bool is_ipv4_addr(const char *s);
bool is_ipv6_addr(const char *s);

/*
 * print_log_l - print using system locale
 * @level : log level
 * @title : log title
 *
 * If @title was NULL, not use log title.
 */
void print_log_l(log_level_t level, const char *title);

#endif /* INCLUDE_ZSR_COMMON_H_ */
