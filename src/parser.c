#define _GNU_SOURCE
#include "psa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

#define PSA_DEFAULT_MAX_LINE_BYTES (8u * 1024u * 1024u)
#define PSA_DEFAULT_MAX_FIELDS     8192u

static void set_err(char *errbuf, size_t errbuf_size, const char *msg)
{
    if (errbuf && errbuf_size > 0)
        snprintf(errbuf, errbuf_size, "%s", msg);
}

static void set_err_if_empty(char *errbuf, size_t errbuf_size, const char *msg)
{
    if (errbuf && errbuf_size > 0 && errbuf[0] == '\0')
        snprintf(errbuf, errbuf_size, "%s", msg);
}

static size_t count_fields_in_line(const char *buf, size_t len)
{
    size_t n = 1;
    for (size_t i = 0; i < len; i++) {
        if (buf[i] == ',')
            n++;
    }
    return n;
}

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

typedef struct {
    int line_no;
    int planogram_key;
    char *header;
    char *version;
    char *scratch;
    size_t scratch_cap;
    char **fields;
    size_t max_fields;
    size_t max_fields_limit;
    psa_record_callback cb;
    void *user_data;
} parser_state_t;

static char *dup_n(const char *s, size_t n)
{
    char *out = malloc(n + 1);
    if (!out)
        return NULL;
    if (n > 0)
        memcpy(out, s, n);
    out[n] = '\0';
    return out;
}

static int ensure_field_capacity(parser_state_t *st, size_t needed_fields,
                                 char *errbuf, size_t errbuf_size)
{
    if (needed_fields <= st->max_fields)
        return PSA_OK;

    if (needed_fields > st->max_fields_limit) {
        if (errbuf && errbuf_size > 0) {
            snprintf(errbuf, errbuf_size,
                     "Line %d has too many fields (%zu > %zu)",
                     st->line_no, needed_fields, st->max_fields_limit);
        }
        return PSA_ERR_OVERFLOW;
    }

    size_t new_max_fields = st->max_fields;
    while (new_max_fields < needed_fields)
        new_max_fields *= 2;
    if (new_max_fields > st->max_fields_limit)
        new_max_fields = st->max_fields_limit;

    char **new_fields = realloc(st->fields, new_max_fields * sizeof(char *));
    if (!new_fields) {
        set_err(errbuf, errbuf_size, "Out of memory");
        return PSA_ERR_NOMEM;
    }

    st->fields = new_fields;
    st->max_fields = new_max_fields;
    return PSA_OK;
}

static int process_record_fields(parser_state_t *st, size_t nfields,
                                 char *errbuf, size_t errbuf_size)
{
    const char *indicator = st->fields[0];
    if (indicator[0] == ';' || indicator[0] == '\0')
        return PSA_OK;

    psa_record_t rec;
    int parsed = 0;

    if (psa_strcasecmp(indicator, "Project") == 0) {
        rec.type = PSA_REC_PROJECT;
        if (psa_parse_project(st->fields, nfields, &rec.rec.project) == 0)
            parsed = 1;
    } else if (psa_strcasecmp(indicator, "Planogram") == 0) {
        rec.type = PSA_REC_PLANOGRAM;
        if (psa_parse_planogram(st->fields, nfields, &rec.rec.planogram) == 0) {
            parsed = 1;
            st->planogram_key = psa_parse_int(st->fields[2], 0);
        }
    } else if (psa_strcasecmp(indicator, "Fixture") == 0) {
        rec.type = PSA_REC_FIXTURE;
        if (psa_parse_fixture(st->fields, nfields, &rec.rec.fixture, st->planogram_key) == 0)
            parsed = 1;
    } else if (psa_strcasecmp(indicator, "Product") == 0) {
        rec.type = PSA_REC_PRODUCT;
        if (psa_parse_product(st->fields, nfields, &rec.rec.product) == 0) {
            parsed = 1;
        } else {
            return PSA_OK;
        }
    } else if (psa_strcasecmp(indicator, "Position") == 0) {
        rec.type = PSA_REC_POSITION;
        if (psa_parse_position(st->fields, nfields, &rec.rec.position, st->planogram_key) == 0)
            parsed = 1;
    } else if (psa_strcasecmp(indicator, "Performance") == 0) {
        rec.type = PSA_REC_PERFORMANCE;
        if (psa_parse_performance(st->fields, nfields, &rec.rec.performance) == 0)
            parsed = 1;
    } else if (psa_strcasecmp(indicator, "Segment") == 0) {
        rec.type = PSA_REC_SEGMENT;
        if (psa_parse_segment(st->fields, nfields, &rec.rec.segment, st->planogram_key) == 0)
            parsed = 1;
    } else if (psa_strcasecmp(indicator, "Drawing") == 0) {
        rec.type = PSA_REC_DRAWING;
        if (psa_parse_drawing(st->fields, nfields, &rec.rec.drawing) == 0)
            parsed = 1;
    } else if (psa_strcasecmp(indicator, "Divider") == 0) {
        rec.type = PSA_REC_DIVIDER;
        if (psa_parse_divider(st->fields, nfields, &rec.rec.divider) == 0)
            parsed = 1;
    } else {
        return PSA_OK;
    }

    if (!parsed) {
        if (errbuf && errbuf_size > 0) {
            snprintf(errbuf, errbuf_size,
                     "Parse error at line %d (indicator: %s)", st->line_no, indicator);
        }
        return PSA_ERR_PARSE;
    }

    if (st->cb(&rec, st->user_data) != 0) {
        set_err_if_empty(errbuf, errbuf_size, "Callback aborted parsing");
        return PSA_ERR_ABORT;
    }

    return PSA_OK;
}

static int process_line(parser_state_t *st, const char *line, size_t len,
                        char *errbuf, size_t errbuf_size)
{
    if (st->line_no == 0) {
        st->header = dup_n(line, len);
        if (!st->header) {
            set_err(errbuf, errbuf_size, "Out of memory");
            return PSA_ERR_NOMEM;
        }
    } else if (st->line_no == 1) {
        st->version = dup_n(line, len);
        if (!st->version) {
            set_err(errbuf, errbuf_size, "Out of memory");
            return PSA_ERR_NOMEM;
        }
    }

    st->line_no++;

    if (len == 0)
        return PSA_OK;

    if (len * 3 + 1 > st->scratch_cap) {
        st->scratch_cap = len * 3 + 1;
        char *new_scratch = realloc(st->scratch, st->scratch_cap);
        if (!new_scratch) {
            set_err(errbuf, errbuf_size, "Out of memory");
            return PSA_ERR_NOMEM;
        }
        st->scratch = new_scratch;
    }

    size_t plen = psa_preprocess_line(line, len, st->scratch, st->scratch_cap);
    size_t needed_fields = count_fields_in_line(st->scratch, plen);

    int rc = ensure_field_capacity(st, needed_fields, errbuf, errbuf_size);
    if (rc != PSA_OK)
        return rc;

    size_t nfields = psa_split_fields(st->scratch, plen, st->fields, st->max_fields);
    if (nfields == 0)
        return PSA_OK;

    return process_record_fields(st, nfields, errbuf, errbuf_size);
}

static int parse_source_init(parser_state_t *st, const psa_parse_limits_t *limits,
                             psa_record_callback cb, void *user_data,
                             char *errbuf, size_t errbuf_size)
{
    memset(st, 0, sizeof(*st));
    st->cb = cb;
    st->user_data = user_data;
    st->max_fields = 512;
    st->max_fields_limit = PSA_DEFAULT_MAX_FIELDS;
    if (limits && limits->max_fields > 0)
        st->max_fields_limit = limits->max_fields;

    st->scratch_cap = 65536;
    st->scratch = malloc(st->scratch_cap);
    if (!st->scratch) {
        set_err(errbuf, errbuf_size, "Out of memory");
        return PSA_ERR_NOMEM;
    }

    st->fields = malloc(st->max_fields * sizeof(char *));
    if (!st->fields) {
        set_err(errbuf, errbuf_size, "Out of memory");
        return PSA_ERR_NOMEM;
    }

    return PSA_OK;
}

static void parse_source_cleanup(parser_state_t *st,
                                 const char **out_header,
                                 const char **out_version)
{
    if (out_header)
        *out_header = st->header;
    else
        free(st->header);

    if (out_version)
        *out_version = st->version;
    else
        free(st->version);

    free(st->scratch);
    free(st->fields);
}

int psa_parse_file_ex(const char *path,
                      const psa_parse_limits_t *limits,
                      const char **out_header,
                      const char **out_version,
                      psa_record_callback cb,
                      void *user_data,
                      char *errbuf, size_t errbuf_size)
{
    if (errbuf && errbuf_size > 0)
        errbuf[0] = '\0';

    if (!path || !cb || (errbuf_size > 0 && !errbuf)) {
        set_err(errbuf, errbuf_size, "Invalid arguments");
        return PSA_ERR_INVALID_ARG;
    }

    size_t max_line_bytes = PSA_DEFAULT_MAX_LINE_BYTES;
    if (limits) {
        if (limits->max_line_bytes > 0)
            max_line_bytes = limits->max_line_bytes;
    }

    FILE *fp = fopen(path, "rb");
    if (!fp) {
        if (errbuf && errbuf_size > 0)
            snprintf(errbuf, errbuf_size, "Cannot open %s: %s", path, strerror(errno));
        return PSA_ERR_IO;
    }

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    int rc = PSA_OK;
    parser_state_t st;

    rc = parse_source_init(&st, limits, cb, user_data, errbuf, errbuf_size);
    if (rc != PSA_OK)
        goto cleanup;

    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        if ((size_t)linelen > max_line_bytes) {
            if (errbuf && errbuf_size > 0) {
                snprintf(errbuf, errbuf_size,
                         "Line %d exceeds max_line_bytes (%zu)",
                         st.line_no + 1, max_line_bytes);
            }
            rc = PSA_ERR_OVERFLOW;
            goto cleanup;
        }

        /* Trim trailing \r and \n */
        size_t len = (size_t)linelen;
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            line[--len] = '\0';

        rc = process_line(&st, line, len, errbuf, errbuf_size);
        if (rc != PSA_OK)
            goto cleanup;
    }

    if (ferror(fp)) {
        if (errbuf && errbuf_size > 0)
            snprintf(errbuf, errbuf_size, "Read error: %s", strerror(errno));
        rc = PSA_ERR_IO;
        goto cleanup;
    }

cleanup:
    if (rc != PSA_OK && errbuf && errbuf_size > 0 && errbuf[0] == '\0') {
        switch (rc) {
            case PSA_ERR_IO:
                set_err(errbuf, errbuf_size, "I/O error");
                break;
            case PSA_ERR_PARSE:
                set_err(errbuf, errbuf_size, "Parse error");
                break;
            case PSA_ERR_ABORT:
                set_err(errbuf, errbuf_size, "Callback aborted parsing");
                break;
            case PSA_ERR_NOMEM:
                set_err(errbuf, errbuf_size, "Out of memory");
                break;
            case PSA_ERR_INVALID_ARG:
                set_err(errbuf, errbuf_size, "Invalid arguments");
                break;
            case PSA_ERR_OVERFLOW:
                set_err(errbuf, errbuf_size, "Safety limit exceeded");
                break;
            default:
                set_err(errbuf, errbuf_size, "Unknown parser error");
                break;
        }
    }

    parse_source_cleanup(&st, out_header, out_version);
    free(line);
    fclose(fp);
    return rc;
}

int psa_parse_buffer_ex(const char *data, size_t data_len,
                        const psa_parse_limits_t *limits,
                        const char **out_header,
                        const char **out_version,
                        psa_record_callback cb,
                        void *user_data,
                        char *errbuf, size_t errbuf_size)
{
    if (errbuf && errbuf_size > 0)
        errbuf[0] = '\0';

    if (((!data) && data_len > 0) || !cb || (errbuf_size > 0 && !errbuf)) {
        set_err(errbuf, errbuf_size, "Invalid arguments");
        return PSA_ERR_INVALID_ARG;
    }

    size_t max_line_bytes = PSA_DEFAULT_MAX_LINE_BYTES;
    if (limits && limits->max_line_bytes > 0)
        max_line_bytes = limits->max_line_bytes;

    parser_state_t st;
    int rc = parse_source_init(&st, limits, cb, user_data, errbuf, errbuf_size);
    if (rc != PSA_OK) {
        parse_source_cleanup(&st, out_header, out_version);
        return rc;
    }

    size_t line_start = 0;
    while (line_start < data_len) {
        size_t i = line_start;
        while (i < data_len && data[i] != '\n')
            i++;

        size_t line_len = i - line_start;
        if (line_len > 0 && data[line_start + line_len - 1] == '\r')
            line_len--;

        if (line_len > max_line_bytes) {
            if (errbuf && errbuf_size > 0) {
                snprintf(errbuf, errbuf_size,
                         "Line %d exceeds max_line_bytes (%zu)",
                         st.line_no + 1, max_line_bytes);
            }
            rc = PSA_ERR_OVERFLOW;
            goto cleanup;
        }

        rc = process_line(&st, data + line_start, line_len, errbuf, errbuf_size);
        if (rc != PSA_OK)
            goto cleanup;

        line_start = i + 1;
    }

cleanup:
    if (rc != PSA_OK && errbuf && errbuf_size > 0 && errbuf[0] == '\0') {
        switch (rc) {
            case PSA_ERR_PARSE:
                set_err(errbuf, errbuf_size, "Parse error");
                break;
            case PSA_ERR_ABORT:
                set_err(errbuf, errbuf_size, "Callback aborted parsing");
                break;
            case PSA_ERR_NOMEM:
                set_err(errbuf, errbuf_size, "Out of memory");
                break;
            case PSA_ERR_INVALID_ARG:
                set_err(errbuf, errbuf_size, "Invalid arguments");
                break;
            case PSA_ERR_OVERFLOW:
                set_err(errbuf, errbuf_size, "Safety limit exceeded");
                break;
            default:
                set_err(errbuf, errbuf_size, "Unknown parser error");
                break;
        }
    }

    parse_source_cleanup(&st, out_header, out_version);
    return rc;
}

int psa_parse_buffer(const char *data, size_t data_len,
                     const char **out_header,
                     const char **out_version,
                     psa_record_callback cb,
                     void *user_data,
                     char *errbuf, size_t errbuf_size)
{
    return psa_parse_buffer_ex(data, data_len, NULL,
                               out_header, out_version,
                               cb, user_data, errbuf, errbuf_size);
}

int psa_parse_file(const char *path,
                   const char **out_header,
                   const char **out_version,
                   psa_record_callback cb,
                   void *user_data,
                   char *errbuf, size_t errbuf_size)
{
    return psa_parse_file_ex(path, NULL, out_header, out_version,
                             cb, user_data, errbuf, errbuf_size);
}
