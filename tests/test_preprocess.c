#define _GNU_SOURCE
#include "psa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FAIL_IF(cond, ...) do { \
    if (cond) { \
        fprintf(stderr, "FAIL: %s:%d: ", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n"); \
        return 1; \
    } \
} while(0)

static int test_preprocess_basic(void)
{
    char src[] = "a,b,c";
    char dst[64];
    size_t n = psa_preprocess_line(src, strlen(src), dst, sizeof(dst));
    char *fields[8];
    size_t nf = psa_split_fields(dst, n, fields, 8);
    FAIL_IF(nf != 3, "expected 3 fields, got %zu", nf);
    FAIL_IF(strcmp(fields[0], "a") != 0, "field 0 mismatch");
    FAIL_IF(strcmp(fields[1], "b") != 0, "field 1 mismatch");
    FAIL_IF(strcmp(fields[2], "c") != 0, "field 2 mismatch");
    return 0;
}

static int test_preprocess_escaped_comma(void)
{
    char src[] = "a\\,b,c";
    char dst[64];
    size_t n = psa_preprocess_line(src, strlen(src), dst, sizeof(dst));
    char *fields[8];
    size_t nf = psa_split_fields(dst, n, fields, 8);
    FAIL_IF(nf != 2, "expected 2 fields, got %zu", nf);
    FAIL_IF(strcmp(fields[0], "a\342\200\232b") != 0, "escaped comma field 0 mismatch: got '%s'", fields[0]);
    FAIL_IF(strcmp(fields[1], "c") != 0, "field 1 mismatch");
    return 0;
}

static int test_preprocess_doubled_backslash(void)
{
    char src[] = "a\\\\b,c";
    char dst[64];
    size_t n = psa_preprocess_line(src, strlen(src), dst, sizeof(dst));
    char *fields[8];
    size_t nf = psa_split_fields(dst, n, fields, 8);
    FAIL_IF(nf != 2, "expected 2 fields, got %zu", nf);
    FAIL_IF(strcmp(fields[0], "a\\b") != 0, "doubled backslash field 0 mismatch: got '%s'", fields[0]);
    return 0;
}

static int test_preprocess_escaped_quote(void)
{
    char src[] = "a\\\"b,c";
    char dst[64];
    size_t n = psa_preprocess_line(src, strlen(src), dst, sizeof(dst));
    char *fields[8];
    size_t nf = psa_split_fields(dst, n, fields, 8);
    FAIL_IF(nf != 2, "expected 2 fields, got %zu", nf);
    FAIL_IF(strcmp(fields[0], "a\\\"b") != 0, "escaped quote field 0 mismatch: got '%s'", fields[0]);
    return 0;
}

static int test_preprocess_empty_field(void)
{
    char src[] = "a,,c";
    char dst[64];
    size_t n = psa_preprocess_line(src, strlen(src), dst, sizeof(dst));
    char *fields[8];
    size_t nf = psa_split_fields(dst, n, fields, 8);
    FAIL_IF(nf != 3, "expected 3 fields, got %zu", nf);
    FAIL_IF(strcmp(fields[0], "a") != 0, "field 0 mismatch");
    FAIL_IF(fields[1][0] != '\0', "field 1 should be empty");
    FAIL_IF(strcmp(fields[2], "c") != 0, "field 2 mismatch");
    return 0;
}

static int test_preprocess_ordered_steps(void)
{
    char src[] = "\\\\,x";
    char dst[64];
    size_t n = psa_preprocess_line(src, strlen(src), dst, sizeof(dst));
    char *fields[8];
    size_t nf = psa_split_fields(dst, n, fields, 8);
    FAIL_IF(nf != 1, "expected 1 field, got %zu", nf);
    FAIL_IF(strcmp(fields[0], "\342\200\232x") != 0,
            "ordered preprocess mismatch: got '%s'", fields[0]);
    return 0;
}

int main(void)
{
    int rc = 0;
    rc |= test_preprocess_basic();
    rc |= test_preprocess_escaped_comma();
    rc |= test_preprocess_doubled_backslash();
    rc |= test_preprocess_escaped_quote();
    rc |= test_preprocess_empty_field();
    rc |= test_preprocess_ordered_steps();
    if (rc == 0)
        printf("preprocess: all tests passed\n");
    return rc;
}
