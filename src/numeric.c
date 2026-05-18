#include "psa.h"
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

/* ------------------------------------------------------------------------- */
/* Helpers                                                                   */
/* ------------------------------------------------------------------------- */

static int is_blank(const char *s)
{
    if (!s)
        return 1;
    while (*s) {
        if (!isspace((unsigned char)*s))
            return 0;
        s++;
    }
    return 1;
}

/* ------------------------------------------------------------------------- */
/* Integer                                                                  */
/* ------------------------------------------------------------------------- */

int psa_parse_int(const char *s, int default_val)
{
    if (!s || is_blank(s))
        return default_val;

    char *end = NULL;
    errno = 0;
    long v = strtol(s, &end, 10);

    if (errno == ERANGE || end == s)
        return default_val;

    /* Reject trailing non-numeric garbage */
    while (*end) {
        if (!isspace((unsigned char)*end))
            return default_val;
        end++;
    }

    return (int)v;
}

/* ------------------------------------------------------------------------- */
/* 64-bit integer                                                           */
/* ------------------------------------------------------------------------- */

int64_t psa_parse_int64(const char *s, int64_t default_val)
{
    if (!s || is_blank(s))
        return default_val;

    char *end = NULL;
    errno = 0;
    long long v = strtoll(s, &end, 10);

    if (errno == ERANGE || end == s)
        return default_val;

    while (*end) {
        if (!isspace((unsigned char)*end))
            return default_val;
        end++;
    }

    return (int64_t)v;
}

/* ------------------------------------------------------------------------- */
/* Floating-point with rounding (half away from zero, 2 decimal places)     */
/* ------------------------------------------------------------------------- */

double psa_parse_double(const char *s, double default_val)
{
    if (!s || is_blank(s))
        return default_val;

    char *end = NULL;
    errno = 0;
    double v = strtod(s, &end);

    if (errno == ERANGE || end == s)
        return default_val;

    while (*end) {
        if (!isspace((unsigned char)*end))
            return default_val;
        end++;
    }

    return round(v * 100.0) / 100.0;
}
