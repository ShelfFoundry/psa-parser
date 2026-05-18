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
    size_t n = 0;
    size_t need = 0;
    int rc = psa_file_meta_to_json("head\"er", "v\\1\n", out, sizeof(out), &n, &need);
    FAIL_IF(rc != PSA_OK, "file meta json unexpectedly failed rc=%d", rc);
    FAIL_IF(need != n + 1, "expected need=n+1, got need=%zu n=%zu", need, n);
    FAIL_IF(strstr(out, "\"header\":\"head\\\"er\"") == NULL,
            "header escaping mismatch: %s", out);
    FAIL_IF(strstr(out, "\"version\":\"v\\\\1\\n\"") == NULL,
            "version escaping mismatch: %s", out);

    rc = psa_file_meta_to_json("a", "b", out, 8, &n, &need);
    FAIL_IF(rc != PSA_ERR_NOSPACE, "small buffer expected PSA_ERR_NOSPACE, got %d", rc);
    FAIL_IF(need > 8 ? 0 : 1, "expected need > 8, got %zu", need);
    return 0;
}

static int test_record_json(void)
{
    psa_record_t rec;
    char out[1024];
    size_t n;
    size_t need;
    int rc;

    memset(&rec, 0, sizeof(rec));
    rec.type = PSA_REC_DIVIDER;
    rec.rec.divider.id = "ID-1";
    rec.rec.divider.desc_text_1 = "desc\"one";
    rec.rec.divider.undef_text_1 = "a\\b";
    rc = psa_record_to_json(&rec, out, sizeof(out), &n, &need);
    FAIL_IF(rc != PSA_OK, "record json unexpectedly failed rc=%d", rc);
    FAIL_IF(strstr(out, "\"type\":\"Divider\"") == NULL, "type missing: %s", out);
    FAIL_IF(strstr(out, "\"id\":\"ID-1\"") == NULL, "id missing: %s", out);
    FAIL_IF(strstr(out, "desc\\\"one") == NULL, "quote escaping missing: %s", out);
    FAIL_IF(strstr(out, "a\\\\b") == NULL, "backslash escaping missing: %s", out);

    rc = psa_record_to_json(&rec, out, 16, &n, &need);
    FAIL_IF(rc != PSA_ERR_NOSPACE, "small buffer expected PSA_ERR_NOSPACE, got %d", rc);
    FAIL_IF(need > 16 ? 0 : 1, "expected need > 16, got %zu", need);

    rec.type = (psa_record_type_t)999;
    rc = psa_record_to_json(&rec, out, sizeof(out), &n, &need);
    FAIL_IF(rc != PSA_ERR_INVALID_ARG, "invalid type expected PSA_ERR_INVALID_ARG, got %d", rc);
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
