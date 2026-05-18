#include "psa.h"
#include <stdio.h>
#include <string.h>

#define FAIL_IF(cond, ...) do { \
    if (cond) { \
        fprintf(stderr, "FAIL: %s:%d: ", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n"); \
        return 1; \
    } \
} while (0)

static int test_file_meta_json(void)
{
    char out[512];
    int n = psa_file_meta_to_json("head\"er", "v\\1\n", out, sizeof(out));
    FAIL_IF(n < 0, "file meta json unexpectedly failed");
    FAIL_IF(strstr(out, "\"header\":\"head\\\"er\"") == NULL,
            "header escaping mismatch: %s", out);
    FAIL_IF(strstr(out, "\"version\":\"v\\\\1\\n\"") == NULL,
            "version escaping mismatch: %s", out);

    n = psa_file_meta_to_json("a", "b", out, 8);
    FAIL_IF(n != -1, "small buffer expected -1, got %d", n);
    return 0;
}

static int test_record_json(void)
{
    psa_record_t rec;
    char out[1024];
    int n;

    memset(&rec, 0, sizeof(rec));
    rec.type = PSA_REC_DIVIDER;
    rec.rec.divider.id = "ID-1";
    rec.rec.divider.desc_text_1 = "desc\"one";
    rec.rec.divider.undef_text_1 = "a\\b";
    n = psa_record_to_json(&rec, out, sizeof(out));
    FAIL_IF(n < 0, "record json unexpectedly failed");
    FAIL_IF(strstr(out, "\"type\":\"Divider\"") == NULL, "type missing: %s", out);
    FAIL_IF(strstr(out, "\"id\":\"ID-1\"") == NULL, "id missing: %s", out);
    FAIL_IF(strstr(out, "desc\\\"one") == NULL, "quote escaping missing: %s", out);
    FAIL_IF(strstr(out, "a\\\\b") == NULL, "backslash escaping missing: %s", out);

    n = psa_record_to_json(&rec, out, 16);
    FAIL_IF(n != -1, "small buffer expected -1, got %d", n);

    rec.type = (psa_record_type_t)999;
    n = psa_record_to_json(&rec, out, sizeof(out));
    FAIL_IF(n != -1, "invalid type expected -1, got %d", n);
    return 0;
}

int main(void)
{
    int rc = 0;
    rc |= test_file_meta_json();
    rc |= test_record_json();
    if (rc == 0)
        printf("json: all tests passed\n");
    return rc;
}
