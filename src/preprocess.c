#include "psa.h"
#include <stdlib.h>
#include <string.h>

/*
 * UTF-8 encoding of U+201A (single low-9 quotation mark).
 */
static const char PSA_COMMA_ESCAPE[3] = { '\xE2', '\x80', '\x9A' };

/*
 * psa_preprocess_line
 *
 * Performs steps 1 and 2 of the line-preprocessing rules:
 *   1. Replace every occurrence of two consecutive backslash characters
 *      with a single backslash.
 *   2. Replace every occurrence of a backslash character immediately followed
 *      by a comma character with the three-byte UTF-8 sequence for U+201A.
 *
 * src     – source buffer (read-only).
 * len     – number of valid bytes in src.
 * dst     – destination buffer (writable).
 * dst_cap – capacity of dst in bytes.  Must be at least len*3 to be safe.
 *
 * Returns the number of bytes written to dst.
 */
size_t psa_preprocess_line(const char *src, size_t len, char *dst, size_t dst_cap)
{
    size_t i = 0;
    size_t mid_len = 0;
    size_t out_len = 0;
    char *mid;

    if (!src || !dst || dst_cap == 0)
        return 0;

    mid = malloc(len + 1);
    if (!mid)
        return 0;

    while (i < len) {
        if (src[i] == '\\' && i + 1 < len && src[i + 1] == '\\') {
            mid[mid_len++] = '\\';
            i += 2;
        } else {
            mid[mid_len++] = src[i++];
        }
    }

    for (i = 0; i < mid_len; i++) {
        if (mid[i] == '\\' && i + 1 < mid_len && mid[i + 1] == ',') {
            if (out_len + 2 < dst_cap) {
                dst[out_len++] = PSA_COMMA_ESCAPE[0];
                dst[out_len++] = PSA_COMMA_ESCAPE[1];
                dst[out_len++] = PSA_COMMA_ESCAPE[2];
            }
            i++;
        } else {
            if (out_len < dst_cap)
                dst[out_len++] = mid[i];
        }
    }

    free(mid);
    return out_len;
}

/*
 * psa_split_fields
 *
 * Splits a pre-processed line on every literal comma character.
 * Fields are written as NUL-terminated pointers into buf.
 *
 * buf        – preprocessed line buffer (will be modified in place).
 * len        – length of preprocessed data.
 * fields     – array of char* to receive field pointers.
 * max_fields – capacity of fields array.
 *
 * Returns the number of fields found.
 */
size_t psa_split_fields(char *buf, size_t len, char **fields, size_t max_fields)
{
    size_t count = 0;
    size_t start = 0;

    for (size_t i = 0; i <= len; i++) {
        if (i == len || buf[i] == ',') {
            if (count < max_fields) {
                buf[i] = '\0';
                fields[count++] = &buf[start];
            }
            start = i + 1;
        }
    }

    return count;
}
