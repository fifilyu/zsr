#include <zsr/common.h>
#include <zsr/log.h>
#include <arpa/inet.h>
#include <math.h>
#include <locale.h>
#include <errno.h>

#define CHECK_NULL(var) { \
    if (var == NULL) { \
        return false; \
    } \
}

bool is_digit(const char *s) {
    int i = 0;
    size_t len = 0;

    CHECK_NULL(s);

    len = strlen(s);

    if (*s == '-') s++;

    while (*s) {
        i++;

        if (*s == '.') {
            // 头尾是'.'的字符串不是数字
            if (i == 1 || i == len)
                return false;
            else
                s++;
        }

        if (!isdigit(*s++)) return false;
    }

    return true;
}

double round_with_prec(const double d, const uint32_t precision) {
    double i = 0.0;
    double y = 0.0;

    if (d == 0)
        return d;

    i = d * pow(10.00, (double)precision);

    if (d > 0)
        i = (int64_t)(i + 0.5);
    else
        i = (int64_t)(i - 0.5);

    const double d_precision = (double)precision;
    y = i * pow(10.00, -d_precision);

    return y;
}

bool is_ipv4_addr(const char *s) {
    CHECK_NULL(s);

    struct sockaddr_in sa;
    return inet_pton(AF_INET, s, &(sa.sin_addr)) == 1;
}

bool is_ipv6_addr(const char *s) {
    CHECK_NULL(s);

    struct sockaddr_in6 sa;
    return inet_pton(AF_INET6, s, &(sa.sin6_addr)) == 1;
}

void print_log_l(log_level_t level, const char *title) {
    char buf[LEN_128] = { 0 };

    strerror_r(errno, buf, LEN_128);

    if (title && strcmp(title, "") != 0)
        print_log(level, "%s: %s\n", title, buf);
    else
        print_log(level, "%s\n", buf);
}
