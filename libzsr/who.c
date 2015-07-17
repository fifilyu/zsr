#include <zsr/zsr.h>
#include <utmp.h>

void get_login_user(list_head_t *head) {
    FILE *fp = NULL;
    char line[LEN_128];
    struct utmp *up = NULL;
    login_user_t *node = NULL;

    setutent();

    while (true) {
        up = getutent();

        if (up == NULL) break;

        if (up->ut_type != USER_PROCESS) continue;

        node = malloc(sizeof(login_user_t));
        memset(node, 0, sizeof(login_user_t));

        strncpy(node->user, up->ut_user, LEN_32);
        strncpy(node->tty, up->ut_line, LEN_32);
        strncpy(node->from, up->ut_host, LEN_256);
        node->login_time = up->ut_time;

        list_add_tail(&(node->list), head);
    }

    endutent();
}
