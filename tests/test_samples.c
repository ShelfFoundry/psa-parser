#define _GNU_SOURCE
#include "psa.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FAIL_IF(cond, ...) do { \
    if (cond) { \
        fprintf(stderr, "FAIL: %s:%d: ", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n"); \
        return 1; \
    } \
} while (0)

typedef struct {
    int idx;
    const char *val;
} field_override_t;

static char *build_record_line(const char *indicator,
                               int nfields,
                               const field_override_t *ov,
                               size_t nov)
{
    const char **fields;
    char *line;
    size_t len = 0;
    size_t pos = 0;
    int i;

    if (!indicator || nfields < 1)
        return NULL;

    fields = calloc((size_t)nfields, sizeof(*fields));
    if (!fields)
        return NULL;

    for (i = 0; i < nfields; i++)
        fields[i] = "";

    fields[0] = indicator;
    for (size_t k = 0; k < nov; k++) {
        if (ov[k].idx >= 0 && ov[k].idx < nfields)
            fields[ov[k].idx] = ov[k].val ? ov[k].val : "";
    }

    for (i = 0; i < nfields; i++) {
        len += strlen(fields[i]);
        if (i > 0)
            len += 1;
    }
    len += 2;

    line = malloc(len);
    if (!line) {
        free(fields);
        return NULL;
    }

    for (i = 0; i < nfields; i++) {
        size_t n;
        if (i > 0)
            line[pos++] = ',';
        n = strlen(fields[i]);
        if (n > 0) {
            memcpy(line + pos, fields[i], n);
            pos += n;
        }
    }
    line[pos++] = '\n';
    line[pos] = '\0';

    free(fields);
    return line;
}

static int write_psa_file(const char *path, const char *const *lines, size_t nlines)
{
    FILE *fp = fopen(path, "wb");
    if (!fp)
        return -1;

    fputs("SYNTHETIC HEADER\n", fp);
    fputs("SYNTHETIC VERSION\n", fp);
    for (size_t i = 0; i < nlines; i++) {
        if (fputs(lines[i], fp) < 0) {
            fclose(fp);
            return -1;
        }
    }

    fclose(fp);
    return 0;
}

typedef struct {
    int counts[9];
    int fixture_planogram_key;
    int position_planogram_key;
    int segment_planogram_key;
    int saw_fixture;
    int saw_position;
    int saw_segment;
} count_state_t;

static int count_cb(psa_record_t *rec, void *user)
{
    count_state_t *st = user;
    st->counts[rec->type]++;

    if (rec->type == PSA_REC_FIXTURE) {
        st->fixture_planogram_key = rec->rec.fixture.planogram_key;
        st->saw_fixture = 1;
    } else if (rec->type == PSA_REC_POSITION) {
        st->position_planogram_key = rec->rec.position.planogram_key;
        st->saw_position = 1;
    } else if (rec->type == PSA_REC_SEGMENT) {
        st->segment_planogram_key = rec->rec.segment.planogram_key;
        st->saw_segment = 1;
    }
    return 0;
}

typedef struct {
    int seen_project;
    int seen_planogram;
    int seen_drawing;
    psa_project_t project;
    psa_planogram_t planogram;
    psa_drawing_t drawing;
} defaults_state_t;

static int defaults_cb(psa_record_t *rec, void *user)
{
    defaults_state_t *st = user;
    if (rec->type == PSA_REC_PROJECT) {
        st->project = rec->rec.project;
        st->seen_project = 1;
    } else if (rec->type == PSA_REC_PLANOGRAM) {
        st->planogram = rec->rec.planogram;
        st->seen_planogram = 1;
    } else if (rec->type == PSA_REC_DRAWING) {
        st->drawing = rec->rec.drawing;
        st->seen_drawing = 1;
    }
    return 0;
}

typedef struct {
    int calls;
} abort_state_t;

static int abort_cb(psa_record_t *rec, void *user)
{
    abort_state_t *st = user;
    (void)rec;
    st->calls++;
    return 1;
}

static int test_synthetic_all_types_and_context(void)
{
    char path[] = "/tmp/psa_synth_all_XXXXXX";
    int fd = mkstemp(path);
    char errbuf[1024] = {0};
    const char *header = NULL;
    const char *version = NULL;
    count_state_t st = {0};
    int rc;
    field_override_t plan_ov[] = { {2, "42"} };

    char *project = build_record_line("Project", 213, NULL, 0);
    char *planogram = build_record_line("Planogram", 229, plan_ov, 1);
    char *fixture = build_record_line("Fixture", 158, NULL, 0);
    char *product = build_record_line("Product", 274, NULL, 0);
    char *position = build_record_line("Position", 167, NULL, 0);
    char *performance = build_record_line("Performance", 150, NULL, 0);
    char *segment = build_record_line("Segment", 12, NULL, 0);
    char *drawing = build_record_line("Drawing", 48, NULL, 0);
    char *divider = build_record_line("Divider", 18, NULL, 0);
    const char *lines[12];

    FAIL_IF(fd < 0, "mkstemp failed");
    close(fd);
    FAIL_IF(!project || !planogram || !fixture || !product || !position ||
            !performance || !segment || !drawing || !divider,
            "line allocation failed");

    lines[0] = "; comment line should be ignored\n";
    lines[1] = "Unknown,1,2,3\n";
    lines[2] = project;
    lines[3] = planogram;
    lines[4] = fixture;
    lines[5] = product;
    lines[6] = position;
    lines[7] = performance;
    lines[8] = segment;
    lines[9] = drawing;
    lines[10] = divider;
    lines[11] = "\n";

    FAIL_IF(write_psa_file(path, lines, 12) != 0, "failed to write synthetic file");

    rc = psa_parse_file(path, &header, &version, count_cb, &st, errbuf, sizeof(errbuf));
    FAIL_IF(rc != PSA_OK, "parse failed: %s", errbuf);
    FAIL_IF(!header || strcmp(header, "SYNTHETIC HEADER") != 0, "bad header capture");
    FAIL_IF(!version || strcmp(version, "SYNTHETIC VERSION") != 0, "bad version capture");

    for (int i = 0; i < 9; i++)
        FAIL_IF(st.counts[i] != 1, "record type %d expected 1, got %d", i, st.counts[i]);

    FAIL_IF(!st.saw_fixture || !st.saw_position || !st.saw_segment,
            "context records not observed");
    FAIL_IF(st.fixture_planogram_key != 42, "fixture planogram_key expected 42, got %d", st.fixture_planogram_key);
    FAIL_IF(st.position_planogram_key != 42, "position planogram_key expected 42, got %d", st.position_planogram_key);
    FAIL_IF(st.segment_planogram_key != 42, "segment planogram_key expected 42, got %d", st.segment_planogram_key);

    free((void *)header);
    free((void *)version);
    unlink(path);
    free(project);
    free(planogram);
    free(fixture);
    free(product);
    free(position);
    free(performance);
    free(segment);
    free(drawing);
    free(divider);
    return 0;
}

static int test_invalid_product_skipped(void)
{
    char path[] = "/tmp/psa_synth_product_skip_XXXXXX";
    int fd = mkstemp(path);
    char errbuf[1024] = {0};
    count_state_t st = {0};
    int rc;
    field_override_t upc_ov[] = { {1, "GOOD-UPC"} };
    char *good_product = build_record_line("Product", 274, upc_ov, 1);
    char *bad_product = build_record_line("Product", 10, NULL, 0);
    const char *lines[2];

    FAIL_IF(fd < 0, "mkstemp failed");
    close(fd);
    FAIL_IF(!good_product || !bad_product, "line allocation failed");

    lines[0] = bad_product;
    lines[1] = good_product;
    FAIL_IF(write_psa_file(path, lines, 2) != 0, "failed to write synthetic file");

    rc = psa_parse_file(path, NULL, NULL, count_cb, &st, errbuf, sizeof(errbuf));
    FAIL_IF(rc != PSA_OK, "parse failed: %s", errbuf);
    FAIL_IF(st.counts[PSA_REC_PRODUCT] != 1,
            "expected 1 valid product after skip, got %d", st.counts[PSA_REC_PRODUCT]);

    unlink(path);
    free(good_product);
    free(bad_product);
    return 0;
}

static int test_invalid_non_product_fails(void)
{
    char path[] = "/tmp/psa_synth_non_product_fail_XXXXXX";
    int fd = mkstemp(path);
    char errbuf[1024] = {0};
    count_state_t st = {0};
    int rc;
    char *project = build_record_line("Project", 213, NULL, 0);
    char *bad_fixture = build_record_line("Fixture", 2, NULL, 0);
    const char *lines[2];

    FAIL_IF(fd < 0, "mkstemp failed");
    close(fd);
    FAIL_IF(!project || !bad_fixture, "line allocation failed");

    lines[0] = project;
    lines[1] = bad_fixture;
    FAIL_IF(write_psa_file(path, lines, 2) != 0, "failed to write synthetic file");

    rc = psa_parse_file(path, NULL, NULL, count_cb, &st, errbuf, sizeof(errbuf));
    FAIL_IF(rc != PSA_ERR_PARSE, "expected PSA_ERR_PARSE, got %d", rc);
    FAIL_IF(strstr(errbuf, "Fixture") == NULL, "expected Fixture in error message, got: %s", errbuf);

    unlink(path);
    free(project);
    free(bad_fixture);
    return 0;
}

static int test_callback_abort(void)
{
    char path[] = "/tmp/psa_synth_abort_XXXXXX";
    int fd = mkstemp(path);
    char errbuf[1024] = {0};
    int rc;
    abort_state_t st = {0};
    char *project = build_record_line("Project", 213, NULL, 0);
    char *planogram = build_record_line("Planogram", 229, NULL, 0);
    const char *lines[2];

    FAIL_IF(fd < 0, "mkstemp failed");
    close(fd);
    FAIL_IF(!project || !planogram, "line allocation failed");

    lines[0] = project;
    lines[1] = planogram;
    FAIL_IF(write_psa_file(path, lines, 2) != 0, "failed to write synthetic file");

    rc = psa_parse_file(path, NULL, NULL, abort_cb, &st, errbuf, sizeof(errbuf));
    FAIL_IF(rc != PSA_ERR_ABORT, "expected PSA_ERR_ABORT, got %d", rc);
    FAIL_IF(st.calls != 1, "expected callback to stop after first call, got %d", st.calls);
    FAIL_IF(strstr(errbuf, "Callback") == NULL,
            "expected callback abort message, got: %s", errbuf);

    unlink(path);
    free(project);
    free(planogram);
    return 0;
}

static int test_spec_defaults(void)
{
    char path[] = "/tmp/psa_synth_defaults_XXXXXX";
    int fd = mkstemp(path);
    char errbuf[1024] = {0};
    defaults_state_t st = {0};
    int rc;
    char *project = build_record_line("Project", 213, NULL, 0);
    char *planogram = build_record_line("Planogram", 229, NULL, 0);
    char *drawing = build_record_line("Drawing", 48, NULL, 0);
    const char *lines[3];

    FAIL_IF(fd < 0, "mkstemp failed");
    close(fd);
    FAIL_IF(!project || !planogram || !drawing, "line allocation failed");

    lines[0] = project;
    lines[1] = planogram;
    lines[2] = drawing;
    FAIL_IF(write_psa_file(path, lines, 3) != 0, "failed to write synthetic file");

    rc = psa_parse_file(path, NULL, NULL, defaults_cb, &st, errbuf, sizeof(errbuf));
    FAIL_IF(rc != PSA_OK, "parse failed: %s", errbuf);
    FAIL_IF(!st.seen_project || !st.seen_planogram || !st.seen_drawing,
            "expected project/planogram/drawing records");

    FAIL_IF(st.project.family_key != -1, "project family_key default expected -1, got %d", st.project.family_key);
    FAIL_IF(fabs(st.planogram.notch_peg[0] - (-1.0)) > 0.0001,
            "planogram notch color default expected -1.0, got %.2f", st.planogram.notch_peg[0]);
    FAIL_IF(st.planogram.alloc_max_target != -1.0,
            "planogram alloc_max_target default expected -1.0, got %.2f", st.planogram.alloc_max_target);
    FAIL_IF(st.planogram.split_ctrl != -1, "planogram split_ctrl default expected -1, got %d", st.planogram.split_ctrl);
    FAIL_IF(st.planogram.final_status != -1, "planogram final_status default expected -1, got %d", st.planogram.final_status);
    FAIL_IF(st.drawing.font_metrics[4] != 400,
            "drawing font weight default expected 400, got %lld", (long long)st.drawing.font_metrics[4]);
    FAIL_IF(st.drawing.font_style[7] != 32,
            "drawing pitch/family default expected 32, got %d", st.drawing.font_style[7]);
    FAIL_IF(strcmp(st.drawing.font_face, "Arial") != 0,
            "drawing font face default expected Arial, got '%s'", st.drawing.font_face ? st.drawing.font_face : "(null)");

    unlink(path);
    free(project);
    free(planogram);
    free(drawing);
    return 0;
}

static int file_contains(const char *path, const char *needle)
{
    char line[2048];
    FILE *fp = fopen(path, "rb");
    if (!fp)
        return 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, needle)) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

static int file_line_count(const char *path)
{
    int lines = 0;
    int ch;
    FILE *fp = fopen(path, "rb");
    if (!fp)
        return -1;

    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\n')
            lines++;
    }

    fclose(fp);
    return lines;
}

static int test_cli_default_document_and_stream_mode(void)
{
    char path[] = "/tmp/psa_synth_cli_modes_XXXXXX";
    char out_doc[] = "/tmp/psa_cli_doc_XXXXXX";
    char out_stream[] = "/tmp/psa_cli_stream_XXXXXX";
    char err_tmp[] = "/tmp/psa_cli_modes_err_XXXXXX";
    char cmd[4096];
    int fd;
    int out_doc_fd;
    int out_stream_fd;
    int err_fd;
    int rc;
    char *project = build_record_line("Project", 213, NULL, 0);
    char *planogram = build_record_line("Planogram", 229, NULL, 0);
    char *product = build_record_line("Product", 274, NULL, 0);
    const char *lines[3];

    fd = mkstemp(path);
    FAIL_IF(fd < 0, "mkstemp failed");
    close(fd);

    out_doc_fd = mkstemp(out_doc);
    out_stream_fd = mkstemp(out_stream);
    err_fd = mkstemp(err_tmp);
    FAIL_IF(out_doc_fd < 0 || out_stream_fd < 0 || err_fd < 0, "mkstemp failed");
    close(out_doc_fd);
    close(out_stream_fd);
    close(err_fd);

    FAIL_IF(!project || !planogram || !product, "line allocation failed");
    lines[0] = project;
    lines[1] = planogram;
    lines[2] = product;
    FAIL_IF(write_psa_file(path, lines, 3) != 0, "failed to write synthetic file");

    snprintf(cmd, sizeof(cmd), "./build/psa-cli '%s' > '%s' 2> '%s'", path, out_doc, err_tmp);
    rc = system(cmd);
    FAIL_IF(rc != 0, "default cli mode failed");
    FAIL_IF(file_line_count(out_doc) != 1, "default mode should emit one document line");
    FAIL_IF(!file_contains(out_doc, "\"projects\":[{"), "default mode missing projects array");

    snprintf(cmd, sizeof(cmd), "./build/psa-cli --stream '%s' > '%s' 2> '%s'", path, out_stream, err_tmp);
    rc = system(cmd);
    FAIL_IF(rc != 0, "stream cli mode failed");
    FAIL_IF(file_line_count(out_stream) != 4, "stream mode should emit metadata + 3 record lines");
    FAIL_IF(!file_contains(out_stream, "\"type\":\"Project\""), "stream mode missing Project record");
    FAIL_IF(!file_contains(out_stream, "\"type\":\"Planogram\""), "stream mode missing Planogram record");
    FAIL_IF(!file_contains(out_stream, "\"type\":\"Product\""), "stream mode missing Product record");

    unlink(path);
    unlink(out_doc);
    unlink(out_stream);
    unlink(err_tmp);
    free(project);
    free(planogram);
    free(product);
    return 0;
}

static int test_cli_summary_with_synthetic_input(void)
{
    char path[] = "/tmp/psa_synth_cli_XXXXXX";
    char out_tmp[] = "/tmp/psa_cli_out_XXXXXX";
    char err_tmp[] = "/tmp/psa_cli_err_XXXXXX";
    char cmd[4096];
    int fd;
    int out_fd;
    int err_fd;
    int rc;
    field_override_t plan_ov[] = { {2, "77"} };
    char *project = build_record_line("Project", 213, NULL, 0);
    char *planogram = build_record_line("Planogram", 229, plan_ov, 1);
    char *product = build_record_line("Product", 274, NULL, 0);
    const char *lines[3];

    fd = mkstemp(path);
    FAIL_IF(fd < 0, "mkstemp failed");
    close(fd);

    out_fd = mkstemp(out_tmp);
    err_fd = mkstemp(err_tmp);
    FAIL_IF(out_fd < 0 || err_fd < 0, "mkstemp failed");
    close(out_fd);
    close(err_fd);

    FAIL_IF(!project || !planogram || !product, "line allocation failed");

    lines[0] = project;
    lines[1] = planogram;
    lines[2] = product;
    FAIL_IF(write_psa_file(path, lines, 3) != 0, "failed to write synthetic file");

    snprintf(cmd, sizeof(cmd), "./build/psa-cli --summary '%s' > '%s' 2> '%s'", path, out_tmp, err_tmp);
    rc = system(cmd);
    FAIL_IF(rc != 0, "psa-cli --summary failed");

    FAIL_IF(file_contains(out_tmp, "{") != 0, "summary mode should not emit JSON records");
    FAIL_IF(!file_contains(err_tmp, "PSA Parse Summary"), "summary header missing");
    FAIL_IF(!file_contains(err_tmp, "Project"), "project count missing in summary");
    FAIL_IF(!file_contains(err_tmp, "Planogram"), "planogram count missing in summary");
    FAIL_IF(!file_contains(err_tmp, "Product"), "product count missing in summary");

    unlink(path);
    unlink(out_tmp);
    unlink(err_tmp);
    free(project);
    free(planogram);
    free(product);
    return 0;
}

static int test_document_json_api(void)
{
    char path[] = "/tmp/psa_synth_doc_api_XXXXXX";
    int fd;
    char errbuf[1024] = {0};
    size_t cap = 256 * 1024;
    char *doc;
    size_t n = 0;
    size_t need = 0;
    int rc;
    char *project = build_record_line("Project", 213, NULL, 0);
    char *planogram = build_record_line("Planogram", 229, NULL, 0);
    char *product = build_record_line("Product", 274, NULL, 0);
    const char *lines[3];

    fd = mkstemp(path);
    FAIL_IF(fd < 0, "mkstemp failed");
    close(fd);
    FAIL_IF(!project || !planogram || !product, "line allocation failed");

    lines[0] = project;
    lines[1] = planogram;
    lines[2] = product;
    FAIL_IF(write_psa_file(path, lines, 3) != 0, "failed to write synthetic file");

    doc = malloc(cap);
    FAIL_IF(!doc, "malloc failed");
    rc = psa_parse_file_to_json_document(path, doc, cap, &n, &need, errbuf, sizeof(errbuf));
    FAIL_IF(rc != PSA_OK, "document API failed: rc=%d err=%s", rc, errbuf);
    FAIL_IF(n == 0, "document API wrote empty output");

    FAIL_IF(strstr(doc, "\"header\":\"SYNTHETIC HEADER\"") == NULL, "missing header in doc json");
    FAIL_IF(strstr(doc, "\"version\":\"SYNTHETIC VERSION\"") == NULL, "missing version in doc json");
    FAIL_IF(strstr(doc, "\"projects\":[{") == NULL, "missing projects array");
    FAIL_IF(strstr(doc, "\"planograms\":[{") == NULL, "missing planograms array");
    FAIL_IF(strstr(doc, "\"products\":[{") == NULL, "missing products array");
    FAIL_IF(strstr(doc, "\"fixtures\":[]") == NULL, "missing empty fixtures array");
    FAIL_IF(strstr(doc, "\"drawings\":[]") == NULL, "missing empty drawings array");

    unlink(path);
    free(project);
    free(planogram);
    free(product);
    free(doc);
    return 0;
}

static int test_invalid_arguments(void)
{
    char errbuf[128] = {0};
    int rc;

    rc = psa_parse_file(NULL, NULL, NULL, count_cb, NULL, errbuf, sizeof(errbuf));
    FAIL_IF(rc != PSA_ERR_INVALID_ARG, "expected PSA_ERR_INVALID_ARG for NULL path, got %d", rc);
    FAIL_IF(errbuf[0] == '\0', "expected errbuf for NULL path");

    errbuf[0] = '\0';
    rc = psa_parse_file("/tmp/does_not_matter", NULL, NULL, NULL, NULL, errbuf, sizeof(errbuf));
    FAIL_IF(rc != PSA_ERR_INVALID_ARG, "expected PSA_ERR_INVALID_ARG for NULL callback, got %d", rc);
    FAIL_IF(errbuf[0] == '\0', "expected errbuf for NULL callback");

    rc = psa_record_to_json(NULL, NULL, 0, NULL, NULL);
    FAIL_IF(rc != PSA_ERR_INVALID_ARG, "expected PSA_ERR_INVALID_ARG for NULL record, got %d", rc);

    rc = psa_file_meta_to_json("h", "v", NULL, 16, NULL, NULL);
    FAIL_IF(rc != PSA_ERR_INVALID_ARG, "expected PSA_ERR_INVALID_ARG for NULL out, got %d", rc);

    errbuf[0] = '\0';
    rc = psa_parse_file_to_json_document(NULL, NULL, 0, NULL, NULL, errbuf, sizeof(errbuf));
    FAIL_IF(rc != PSA_ERR_INVALID_ARG, "expected PSA_ERR_INVALID_ARG for NULL path in doc API, got %d", rc);
    FAIL_IF(errbuf[0] == '\0', "expected errbuf for doc API invalid args");

    return 0;
}

int main(void)
{
    int rc = 0;
    rc |= test_synthetic_all_types_and_context();
    rc |= test_invalid_product_skipped();
    rc |= test_invalid_non_product_fails();
    rc |= test_callback_abort();
    rc |= test_spec_defaults();
    rc |= test_cli_default_document_and_stream_mode();
    rc |= test_cli_summary_with_synthetic_input();
    rc |= test_document_json_api();
    rc |= test_invalid_arguments();

    if (rc == 0)
        printf("samples: all synthetic tests passed\n");
    return rc;
}
