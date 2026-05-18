#include "psa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#define FAIL_IF(cond, ...) do { \
    if (cond) { \
        fprintf(stderr, "FAIL: %s:%d: ", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n"); \
        return 1; \
    } \
} while(0)

static int test_int(void)
{
    FAIL_IF(psa_parse_int("42", 0) != 42, "basic int");
    FAIL_IF(psa_parse_int("", 7) != 7, "empty int default");
    FAIL_IF(psa_parse_int(NULL, 7) != 7, "null int default");
    FAIL_IF(psa_parse_int("abc", 7) != 7, "invalid int default");
    FAIL_IF(psa_parse_int("  123  ", 0) != 123, "whitespace int");
    FAIL_IF(psa_parse_int("-5", 0) != -5, "negative int");
    return 0;
}

static int test_double(void)
{
    double v;
    v = psa_parse_double("3.14159", 0.0);
    FAIL_IF(fabs(v - 3.14) > 0.001, "double rounding: got %.4f", v);

    v = psa_parse_double("2.345", 0.0);
    FAIL_IF(fabs(v - 2.35) > 0.0001, "double half-up: got %.4f", v);

    v = psa_parse_double("-2.345", 0.0);
    FAIL_IF(fabs(v - (-2.35)) > 0.0001, "double negative half-away: got %.4f", v);

    v = psa_parse_double("", 1.5);
    FAIL_IF(v != 1.5, "empty double default");

    v = psa_parse_double(NULL, 1.5);
    FAIL_IF(v != 1.5, "null double default");

    v = psa_parse_double("abc", 1.5);
    FAIL_IF(v != 1.5, "invalid double default");

    v = psa_parse_double("44418.38124999999854480848", 0.0);
    FAIL_IF(fabs(v - 44418.38) > 0.001, "high precision double: got %.4f", v);
    return 0;
}

static int test_int64(void)
{
    FAIL_IF(psa_parse_int64("9223372036854775807", 0) != INT64_C(9223372036854775807), "int64 max");
    FAIL_IF(psa_parse_int64("-9223372036854775808", 0) != INT64_C(-9223372036854775807)-1, "int64 min");
    FAIL_IF(psa_parse_int64("", 5) != 5, "int64 empty default");
    return 0;
}

int main(void)
{
    int rc = 0;
    rc |= test_int();
    rc |= test_double();
    rc |= test_int64();
    if (rc == 0)
        printf("numeric: all tests passed\n");
    return rc;
}
