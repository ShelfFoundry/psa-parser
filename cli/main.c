#define _GNU_SOURCE
#include "psa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define JSON_BUF_SIZE (256 * 1024)

typedef struct {
    int counts[9];
    int summary_mode;
} cli_state_t;

static int record_callback(psa_record_t *rec, void *user_data)
{
    cli_state_t *st = user_data;
    st->counts[rec->type]++;

    if (st->summary_mode)
        return 0;

    char *buf = malloc(JSON_BUF_SIZE);
    if (!buf) {
        fprintf(stderr, "Out of memory\n");
        return -1;
    }

    size_t n = 0;
    size_t needed = 0;
    int rc = psa_record_to_json(rec, buf, JSON_BUF_SIZE, &n, &needed);
    if (rc != PSA_OK) {
        if (rc == PSA_ERR_NOSPACE)
            fprintf(stderr, "JSON serialization failed (need %zu bytes)\n", needed);
        else
            fprintf(stderr, "JSON serialization failed (rc=%d)\n", rc);
        free(buf);
        return -1;
    }

    fwrite(buf, 1, n, stdout);
    fputc('\n', stdout);
    free(buf);
    return 0;
}

static void print_summary(const char *header, const char *version, cli_state_t *st)
{
    const char *names[] = {
        "Project", "Planogram", "Fixture", "Product",
        "Position", "Performance", "Segment", "Drawing", "Divider"
    };
    fprintf(stderr, "PSA Parse Summary\n");
    fprintf(stderr, "  Header:  %s\n", header ? header : "(none)");
    fprintf(stderr, "  Version: %s\n", version ? version : "(none)");
    for (int i = 0; i < 9; i++) {
        if (st->counts[i] > 0)
            fprintf(stderr, "  %-12s: %d\n", names[i], st->counts[i]);
    }
}

static char *read_line(FILE *fp)
{
    char *line = NULL;
    size_t cap = 0;
    ssize_t n = getline(&line, &cap, fp);
    if (n < 0) {
        free(line);
        return NULL;
    }
    /* trim trailing \r\n */
    while (n > 0 && (line[n-1] == '\n' || line[n-1] == '\r'))
        line[--n] = '\0';
    return line;
}

int main(int argc, char **argv)
{
    int summary_mode = 0;
    int stream_mode = 0;
    const char *path = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--summary") == 0 || strcmp(argv[i], "-s") == 0) {
            summary_mode = 1;
        } else if (strcmp(argv[i], "--stream") == 0) {
            stream_mode = 1;
        } else if (strcmp(argv[i], "--document") == 0 || strcmp(argv[i], "-j") == 0) {
            stream_mode = 0;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("Usage: %s [--summary|--stream] <file.psa>\n", argv[0]);
            printf("  Default output is one top-level JSON document.\n");
            printf("  --summary  Print parse statistics to stderr instead of records to stdout.\n");
            printf("  --stream   Emit one JSON object per line (JSON Lines format).\n");
            return 0;
        } else {
            path = argv[i];
        }
    }

    if (!path) {
        fprintf(stderr, "Usage: %s [--summary|--stream] <file.psa>\n", argv[0]);
        return 1;
    }

    if (!summary_mode && !stream_mode) {
        size_t doc_cap = 1024 * 1024;
        char *doc = malloc(doc_cap);
        char errbuf[1024] = {0};
        if (!doc) {
            fprintf(stderr, "Out of memory\n");
            return 1;
        }

        int rc;
        size_t written = 0;
        size_t needed = 0;
        for (;;) {
            rc = psa_parse_file_to_json_document(path, doc, doc_cap,
                                                 &written, &needed,
                                                 errbuf, sizeof(errbuf));
            if (rc == PSA_OK)
                break;
            if (rc != PSA_ERR_NOSPACE) {
                if (errbuf[0] != '\0')
                    fprintf(stderr, "Parse error: %s\n", errbuf);
                else
                    fprintf(stderr, "Parse failed with rc=%d\n", rc);
                free(doc);
                return 1;
            }
            if (doc_cap >= 64 * 1024 * 1024) {
                fprintf(stderr, "JSON document too large\n");
                free(doc);
                return 1;
            }
            if (needed > doc_cap)
                doc_cap = needed;
            else
                doc_cap *= 2;
            char *next = realloc(doc, doc_cap);
            if (!next) {
                fprintf(stderr, "Out of memory\n");
                free(doc);
                return 1;
            }
            doc = next;
        }

        fwrite(doc, 1, written, stdout);
        fputc('\n', stdout);
        free(doc);
        return 0;
    }

    /* Pre-read header and version so we can emit metadata first. */
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        fprintf(stderr, "Cannot open %s\n", path);
        return 1;
    }
    char *header = read_line(fp);
    char *version = read_line(fp);
    fclose(fp);

    cli_state_t st = {0};
    st.summary_mode = summary_mode;
    char errbuf[1024] = {0};

    if (!summary_mode) {
        char meta[4096];
        size_t n = 0;
        int mrc = psa_file_meta_to_json(header ? header : "",
                                        version ? version : "",
                                        meta, sizeof(meta),
                                        &n, NULL);
        if (mrc == PSA_OK) {
            fwrite(meta, 1, n, stdout);
            fputc('\n', stdout);
        }
    }

    int rc = psa_parse_file(path, NULL, NULL,
                            record_callback, &st,
                            errbuf, sizeof(errbuf));

    if (rc != PSA_OK) {
        fprintf(stderr, "Parse error: %s\n", errbuf);
        free(header);
        free(version);
        return 1;
    }

    if (summary_mode) {
        print_summary(header, version, &st);
    }

    free(header);
    free(version);
    return 0;
}
