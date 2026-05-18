#define _GNU_SOURCE
#include "psa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

/* ------------------------------------------------------------------------- */
/* Internal forward declarations                                             */
/* ------------------------------------------------------------------------- */

extern int psa_parse_project(char **fields, size_t nfields, psa_project_t *p);
extern int psa_parse_planogram(char **fields, size_t nfields, psa_planogram_t *p);
extern int psa_parse_fixture(char **fields, size_t nfields, psa_fixture_t *p, int planogram_key);
extern int psa_parse_product(char **fields, size_t nfields, psa_product_t *p);
extern int psa_parse_position(char **fields, size_t nfields, psa_position_t *p, int planogram_key);
extern int psa_parse_performance(char **fields, size_t nfields, psa_performance_t *p);
extern int psa_parse_segment(char **fields, size_t nfields, psa_segment_t *p, int planogram_key);
extern int psa_parse_drawing(char **fields, size_t nfields, psa_drawing_t *p);
extern int psa_parse_divider(char **fields, size_t nfields, psa_divider_t *p);

/* ------------------------------------------------------------------------- */
/* Case-insensitive string comparison                                         */
/* ------------------------------------------------------------------------- */

static int psa_strcasecmp(const char *a, const char *b)
{
    while (*a && *b) {
        unsigned char ca = (unsigned char)tolower((unsigned char)*a);
        unsigned char cb = (unsigned char)tolower((unsigned char)*b);
        if (ca != cb)
            return (int)ca - (int)cb;
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

/* ------------------------------------------------------------------------- */
/* Main parser                                                               */
/* ------------------------------------------------------------------------- */

int psa_parse_file(const char *path,
                   const char **out_header,
                   const char **out_version,
                   psa_record_callback cb,
                   void *user_data,
                   char *errbuf, size_t errbuf_size)
{
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        if (errbuf && errbuf_size > 0) {
            snprintf(errbuf, errbuf_size, "Cannot open %s: %s", path, strerror(errno));
        }
        return PSA_ERR_IO;
    }

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    int line_no = 0;
    int planogram_key = 0;
    char *header = NULL;
    char *version = NULL;
    int rc = PSA_OK;
    char **fields = NULL;

    /* Large scratch buffer for preprocessing (2x max line length) */
    size_t scratch_cap = 65536;
    char *scratch = malloc(scratch_cap);
    if (!scratch) {
        rc = PSA_ERR_IO;
        if (errbuf && errbuf_size > 0)
            snprintf(errbuf, errbuf_size, "Out of memory");
        goto cleanup;
    }

    /* Field pointer array */
    size_t max_fields = 512;
    fields = malloc(max_fields * sizeof(char *));
    if (!fields) {
        rc = PSA_ERR_IO;
        if (errbuf && errbuf_size > 0)
            snprintf(errbuf, errbuf_size, "Out of memory");
        goto cleanup;
    }

    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        /* Trim trailing \r and \n */
        size_t len = (size_t)linelen;
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            line[--len] = '\0';

        if (line_no == 0) {
            header = strdup(line);
        } else if (line_no == 1) {
            version = strdup(line);
        }

        line_no++;

        /* Skip empty lines */
        if (len == 0)
            continue;

        /* Ensure scratch buffer is large enough (needs 3x original for \, -> U+201A) */
        if (len * 3 + 1 > scratch_cap) {
            scratch_cap = len * 3 + 1;
            char *new_scratch = realloc(scratch, scratch_cap);
            if (!new_scratch) {
                rc = PSA_ERR_IO;
                if (errbuf && errbuf_size > 0)
                    snprintf(errbuf, errbuf_size, "Out of memory");
                goto cleanup;
            }
            scratch = new_scratch;
        }

        size_t plen = psa_preprocess_line(line, len, scratch, scratch_cap);

        size_t nfields = psa_split_fields(scratch, plen, fields, max_fields);
        if (nfields == 0)
            continue;

        const char *indicator = fields[0];
        if (indicator[0] == ';' || indicator[0] == '\0')
            continue;   /* comment/preamble or empty indicator */

        psa_record_t rec;
        int parsed = 0;

        if (psa_strcasecmp(indicator, "Project") == 0) {
            rec.type = PSA_REC_PROJECT;
            if (psa_parse_project(fields, nfields, &rec.rec.project) == 0)
                parsed = 1;
        } else if (psa_strcasecmp(indicator, "Planogram") == 0) {
            rec.type = PSA_REC_PLANOGRAM;
            if (psa_parse_planogram(fields, nfields, &rec.rec.planogram) == 0) {
                parsed = 1;
                /* Update planogram context key from column 2 (string, parsed as int) */
                planogram_key = psa_parse_int(fields[2], 0);
            }
        } else if (psa_strcasecmp(indicator, "Fixture") == 0) {
            rec.type = PSA_REC_FIXTURE;
            if (psa_parse_fixture(fields, nfields, &rec.rec.fixture, planogram_key) == 0)
                parsed = 1;
        } else if (psa_strcasecmp(indicator, "Product") == 0) {
            rec.type = PSA_REC_PRODUCT;
            if (psa_parse_product(fields, nfields, &rec.rec.product) == 0) {
                parsed = 1;
            } else {
                /* Invalid Product rows are skipped; parsing continues */
                continue;
            }
        } else if (psa_strcasecmp(indicator, "Position") == 0) {
            rec.type = PSA_REC_POSITION;
            if (psa_parse_position(fields, nfields, &rec.rec.position, planogram_key) == 0)
                parsed = 1;
        } else if (psa_strcasecmp(indicator, "Performance") == 0) {
            rec.type = PSA_REC_PERFORMANCE;
            if (psa_parse_performance(fields, nfields, &rec.rec.performance) == 0)
                parsed = 1;
        } else if (psa_strcasecmp(indicator, "Segment") == 0) {
            rec.type = PSA_REC_SEGMENT;
            if (psa_parse_segment(fields, nfields, &rec.rec.segment, planogram_key) == 0)
                parsed = 1;
        } else if (psa_strcasecmp(indicator, "Drawing") == 0) {
            rec.type = PSA_REC_DRAWING;
            if (psa_parse_drawing(fields, nfields, &rec.rec.drawing) == 0)
                parsed = 1;
        } else if (psa_strcasecmp(indicator, "Divider") == 0) {
            rec.type = PSA_REC_DIVIDER;
            if (psa_parse_divider(fields, nfields, &rec.rec.divider) == 0)
                parsed = 1;
        } else {
            /* Unknown indicator: ignore */
            continue;
        }

        if (!parsed) {
            /* Invalid rows for non-Product types terminate parsing */
            if (errbuf && errbuf_size > 0) {
                snprintf(errbuf, errbuf_size,
                         "Parse error at line %d (indicator: %s)", line_no, indicator);
            }
            rc = PSA_ERR_PARSE;
            goto cleanup;
        }

        int cb_rc = cb(&rec, user_data);
        if (cb_rc != 0) {
            rc = PSA_ERR_ABORT;
            goto cleanup;
        }
    }

    if (ferror(fp)) {
        if (errbuf && errbuf_size > 0)
            snprintf(errbuf, errbuf_size, "Read error: %s", strerror(errno));
        rc = PSA_ERR_IO;
        goto cleanup;
    }

cleanup:
    if (out_header) *out_header = header;
    else free(header);
    if (out_version) *out_version = version;
    else free(version);
    free(scratch);
    free(fields);
    free(line);
    fclose(fp);
    return rc;
}
