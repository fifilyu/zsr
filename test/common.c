#include <stdio.h>
#include <check.h>

#include "zsr/common.h"

START_TEST
(test_is_digit) {
    bool ret = false;

    ret = is_digit(NULL);
    ck_assert(ret == false);

    ret = is_digit("a23a");
    ck_assert(ret == false);

    ret = is_digit(".123");
    ck_assert(ret == false);

    ret = is_digit("123.");
    ck_assert(ret == false);

    ret = is_digit("1.23");
    ck_assert(ret == true);

    ret = is_digit("-1.23");
    ck_assert(ret == true);

    ret = is_digit("123");
    ck_assert(ret == true);

    ret = is_digit("-123");
    ck_assert(ret == true);
}
END_TEST

START_TEST
(test_round_with_prec) {
    double ret = 0.0;

    ret = round_with_prec(0, 2);
    ck_assert(ret == 0);

    ret = round_with_prec(1.2345, 2);
    ck_assert(ret == 1.23);

    ret = round_with_prec(1.2678, 2);
    ck_assert(ret == 1.27);

    ret = round_with_prec(-1.2345, 2);
    ck_assert(ret == -1.23);

    ret = round_with_prec(-1.2678, 2);
    ck_assert(ret == -1.27);
}
END_TEST

START_TEST
(test_is_ipv4_addr) {
    bool ret = false;

    ret = is_ipv4_addr(NULL);
    ck_assert(ret == false);

    ret = is_ipv4_addr("abc");
    ck_assert(ret == false);

    ret = is_ipv4_addr("123");
    ck_assert(ret == false);

    ret = is_ipv4_addr("1.2.3.4");
    ck_assert(ret == true);

    ret = is_ipv4_addr("8.8.8.8");
    ck_assert(ret == true);

    ret = is_ipv4_addr("255.255.255.255");
    ck_assert(ret == true);
}
END_TEST

START_TEST
(test_is_ipv6_addr) {
    bool ret = false;

    ret = is_ipv6_addr(NULL);
    ck_assert(ret == false);

    ret = is_ipv6_addr("abc");
    ck_assert(ret == false);

    ret = is_ipv6_addr("123");
    ck_assert(ret == false);

    ret = is_ipv6_addr("2001:DB8:2de:0:0:0:0:e13");
    ck_assert(ret == true);
}
END_TEST

Suite* common_suite(void) {
    Suite *suite = suite_create("common_suite");
    TCase *tcase = tcase_create("common_case");

    tcase_add_test(tcase, test_is_digit);
    tcase_add_test(tcase, test_round_with_prec);
    tcase_add_test(tcase, test_is_ipv4_addr);
    tcase_add_test(tcase, test_is_ipv6_addr);

    suite_add_tcase(suite, tcase);

    return suite;
}

int main (int argc, char *argv[]) {
    int number_failed;

    Suite *suite = common_suite();
    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return number_failed;
}
