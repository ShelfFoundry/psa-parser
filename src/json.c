#include "psa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PSA_DEFAULT_MAX_DOCUMENT_BYTES (64u * 1024u * 1024u)

/* ======================================================================== */
/* Error helpers                                                            */
/* ======================================================================== */

static void set_err(char *errbuf, size_t errbuf_size, const char *msg)
{
    if (errbuf && errbuf_size > 0)
        snprintf(errbuf, errbuf_size, "%s", msg);
}

static void set_err_for_rc_if_empty(int rc, char *errbuf, size_t errbuf_size)
{
    if (!errbuf || errbuf_size == 0 || errbuf[0] != '\0')
        return;

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
        case PSA_ERR_NOSPACE:
            set_err(errbuf, errbuf_size, "Output buffer too small");
            break;
        case PSA_ERR_NONFINITE:
            set_err(errbuf, errbuf_size, "Encountered non-finite numeric value");
            break;
        default:
            set_err(errbuf, errbuf_size, "Unknown JSON error");
            break;
    }
}

/* ======================================================================== */
/* JSON output buffer                                                       */
/* ======================================================================== */

typedef struct {
    char   *buf;
    size_t  cap;
    size_t  len;
    int     err;
    int     needs_comma;
} json_buf_t;

static void json_buf_append_bytes(json_buf_t *b, const char *s, size_t n)
{
    if (b->err != PSA_OK)
        return;

    if (b->len > SIZE_MAX - n) {
        b->err = PSA_ERR_OVERFLOW;
        return;
    }

    size_t start = b->len;
    b->len += n;

    if (!b->buf || b->cap == 0)
        return;

    if (start >= b->cap) {
        b->err = PSA_ERR_NOSPACE;
        return;
    }

    size_t avail = b->cap - start;
    if (avail <= 1) {
        b->err = PSA_ERR_NOSPACE;
        return;
    }

    size_t to_copy = n;
    if (to_copy > avail - 1)
        to_copy = avail - 1;

    memcpy(b->buf + start, s, to_copy);
    b->buf[start + to_copy] = '\0';

    if (to_copy < n)
        b->err = PSA_ERR_NOSPACE;
}

static void json_buf_append_char(json_buf_t *b, char c)
{
    json_buf_append_bytes(b, &c, 1);
}

static void json_buf_append_cstr(json_buf_t *b, const char *s)
{
    if (!s)
        return;
    json_buf_append_bytes(b, s, strlen(s));
}

static void json_buf_append_int(json_buf_t *b, int v)
{
    char t[32];
    int n = snprintf(t, sizeof(t), "%d", v);
    if (n < 0 || (size_t)n >= sizeof(t)) {
        b->err = PSA_ERR_OVERFLOW;
        return;
    }
    json_buf_append_bytes(b, t, (size_t)n);
}

static void json_buf_append_int64(json_buf_t *b, int64_t v)
{
    char t[32];
    int n = snprintf(t, sizeof(t), "%lld", (long long)v);
    if (n < 0 || (size_t)n >= sizeof(t)) {
        b->err = PSA_ERR_OVERFLOW;
        return;
    }
    json_buf_append_bytes(b, t, (size_t)n);
}

static void json_buf_append_double(json_buf_t *b, double v)
{
    if (!isfinite(v)) {
        b->err = PSA_ERR_NONFINITE;
        return;
    }

    char t[64];
    int n = snprintf(t, sizeof(t), "%.15g", v);
    if (n < 0 || (size_t)n >= sizeof(t)) {
        b->err = PSA_ERR_OVERFLOW;
        return;
    }
    json_buf_append_bytes(b, t, (size_t)n);
}

static void json_buf_append_escaped(json_buf_t *b, const char *s)
{
    if (!s)
        return;

    for (const unsigned char *p = (const unsigned char *)s; *p; p++) {
        unsigned char c = *p;
        if (c == '"') {
            json_buf_append_cstr(b, "\\\"");
        } else if (c == '\\') {
            json_buf_append_cstr(b, "\\\\");
        } else if (c == '\b') {
            json_buf_append_cstr(b, "\\b");
        } else if (c == '\f') {
            json_buf_append_cstr(b, "\\f");
        } else if (c == '\n') {
            json_buf_append_cstr(b, "\\n");
        } else if (c == '\r') {
            json_buf_append_cstr(b, "\\r");
        } else if (c == '\t') {
            json_buf_append_cstr(b, "\\t");
        } else if (c < 0x20) {
            char t[8];
            int n = snprintf(t, sizeof(t), "\\u%04x", c);
            if (n < 0 || (size_t)n >= sizeof(t)) {
                b->err = PSA_ERR_OVERFLOW;
                return;
            }
            json_buf_append_bytes(b, t, (size_t)n);
        } else {
            json_buf_append_char(b, (char)c);
        }
        if (b->err != PSA_OK)
            return;
    }
}

/* ======================================================================== */
/* JSON structural helpers                                                  */
/* ======================================================================== */

static void json_write_object_start(json_buf_t *b)
{
    json_buf_append_char(b, '{');
    b->needs_comma = 0;
}

static void json_write_object_end(json_buf_t *b)
{
    json_buf_append_char(b, '}');
    b->needs_comma = 0;
}

static void json_write_key(json_buf_t *b, const char *key)
{
    if (b->err != PSA_OK)
        return;
    if (b->needs_comma)
        json_buf_append_char(b, ',');
    json_buf_append_char(b, '"');
    json_buf_append_cstr(b, key);
    json_buf_append_char(b, '"');
    json_buf_append_char(b, ':');
    b->needs_comma = 1;
}

static void json_write_field_str(json_buf_t *b, const char *key, const char *val)
{
    json_write_key(b, key);
    json_buf_append_char(b, '"');
    json_buf_append_escaped(b, val);
    json_buf_append_char(b, '"');
}

static void json_write_field_int(json_buf_t *b, const char *key, int val)
{
    json_write_key(b, key);
    json_buf_append_int(b, val);
}

static void json_write_field_double(json_buf_t *b, const char *key, double val)
{
    json_write_key(b, key);
    json_buf_append_double(b, val);
}

static void json_write_field_int64(json_buf_t *b, const char *key, int64_t val)
{
    json_write_key(b, key);
    json_buf_append_int64(b, val);
}

static void json_write_field_int_array(json_buf_t *b, const char *key,
                                       const int *arr, int n)
{
    json_write_key(b, key);
    json_buf_append_char(b, '[');
    for (int i = 0; i < n; i++) {
        if (i > 0)
            json_buf_append_char(b, ',');
        json_buf_append_int(b, arr[i]);
    }
    json_buf_append_char(b, ']');
}

static void json_write_field_double_array(json_buf_t *b, const char *key,
                                          const double *arr, int n)
{
    json_write_key(b, key);
    json_buf_append_char(b, '[');
    for (int i = 0; i < n; i++) {
        if (i > 0)
            json_buf_append_char(b, ',');
        json_buf_append_double(b, arr[i]);
    }
    json_buf_append_char(b, ']');
}

static void json_write_field_int64_array(json_buf_t *b, const char *key,
                                         const int64_t *arr, int n)
{
    json_write_key(b, key);
    json_buf_append_char(b, '[');
    for (int i = 0; i < n; i++) {
        if (i > 0)
            json_buf_append_char(b, ',');
        json_buf_append_int64(b, arr[i]);
    }
    json_buf_append_char(b, ']');
}

static void json_write_field_str_array(json_buf_t *b, const char *key,
                                       const char *const *arr, int n)
{
    json_write_key(b, key);
    json_buf_append_char(b, '[');
    for (int i = 0; i < n; i++) {
        if (i > 0)
            json_buf_append_char(b, ',');
        json_buf_append_char(b, '"');
        json_buf_append_escaped(b, arr[i]);
        json_buf_append_char(b, '"');
    }
    json_buf_append_char(b, ']');
}

/* ======================================================================== */
/* Per-record serializers                                                   */
/* ======================================================================== */

static int serialize_project(json_buf_t *b, const psa_project_t *p)
{
    json_write_object_start(b);
    json_write_field_str(b, "type", "Project");
    json_write_field_str(b, "display_name", p->display_name);
    json_write_field_str(b, "key_text", p->key_text);
    json_write_field_int(b, "primary_key", p->primary_key);
    json_write_field_str(b, "layout_file", p->layout_file);
    json_write_field_int(b, "movement_period", p->movement_period);
    json_write_field_double(b, "case_multiple", p->case_multiple);
    json_write_field_double(b, "days_supply", p->days_supply);
    json_write_field_int(b, "demand_cycle", p->demand_cycle);
    json_write_field_double(b, "peak_safety", p->peak_safety);
    json_write_field_double(b, "backroom_stock", p->backroom_stock);
    json_write_field_str(b, "peg_profile", p->peg_profile);
    json_write_field_int(b, "measurement_mode", p->measurement_mode);
    json_write_field_int(b, "num_stores", p->num_stores);
    json_write_field_int_array(b, "merch_x", p->merch_x, 9);
    json_write_field_int_array(b, "merch_y", p->merch_y, 9);
    json_write_field_int_array(b, "merch_z", p->merch_z, 9);
    json_write_field_double_array(b, "demand", p->demand, 28);
    json_write_field_int_array(b, "inv_model_opts", p->inv_model_opts, 6);
    json_write_field_double_array(b, "num_ext", p->num_ext, PSA_PROJECT_NUM_SLOTS);
    json_write_field_str_array(b, "text_ext", p->text_ext, PSA_PROJECT_TEXT_SLOTS);
    json_write_field_int_array(b, "flag_ext", p->flag_ext, PSA_PROJECT_FLAG_SLOTS);
    json_write_field_str(b, "notes", p->notes);
    json_write_field_int(b, "changed", p->changed);
    json_write_field_int64_array(b, "ext_db_keys", p->ext_db_keys, 10);
    json_write_field_int_array(b, "perf_override", p->perf_override, 4);
    json_write_field_str(b, "status", p->status);
    json_write_field_int_array(b, "date_slots", p->date_slots, 8);
    json_write_field_str(b, "created_by", p->created_by);
    json_write_field_str(b, "modified_by", p->modified_by);
    json_write_field_int(b, "inv_mode", p->inv_mode);
    json_write_field_str(b, "delivery_schedule", p->delivery_schedule);
    json_write_field_str(b, "custom_payload", p->custom_payload);
    json_write_field_int(b, "family_key", p->family_key);
    json_write_object_end(b);
    return b->err;
}

static int serialize_planogram(json_buf_t *b, const psa_planogram_t *p)
{
    json_write_object_start(b);
    json_write_field_str(b, "type", "Planogram");
    json_write_field_str(b, "name", p->name);
    json_write_field_str(b, "key_text", p->key_text);
    json_write_field_double(b, "width", p->width);
    json_write_field_double(b, "height", p->height);
    json_write_field_double(b, "depth", p->depth);
    json_write_field_int(b, "display_color", p->display_color);
    json_write_field_double(b, "back_depth", p->back_depth);
    json_write_field_int(b, "draw_back", p->draw_back);
    json_write_field_double(b, "base_width", p->base_width);
    json_write_field_double(b, "base_height", p->base_height);
    json_write_field_double(b, "base_depth", p->base_depth);
    json_write_field_int(b, "draw_base", p->draw_base);
    json_write_field_int(b, "base_color", p->base_color);
    json_write_field_double_array(b, "notch_peg", p->notch_peg, 8);
    json_write_field_int(b, "traffic_flow", p->traffic_flow);
    json_write_field_int(b, "auto_created", p->auto_created);
    json_write_field_str(b, "shape_ref", p->shape_ref);
    json_write_field_str(b, "bitmap_ref", p->bitmap_ref);
    json_write_field_int_array(b, "merch_x", p->merch_x, 9);
    json_write_field_int_array(b, "merch_y", p->merch_y, 9);
    json_write_field_int_array(b, "merch_z", p->merch_z, 9);
    json_write_field_double(b, "combined_perf", p->combined_perf);
    json_write_field_int(b, "store_count", p->store_count);
    json_write_field_double(b, "notch_width", p->notch_width);
    json_write_field_str_array(b, "text_ext", p->text_ext, PSA_PLANOGRAM_TEXT_SLOTS);
    json_write_field_double_array(b, "num_ext", p->num_ext, PSA_PLANOGRAM_NUM_SLOTS);
    json_write_field_int_array(b, "flag_ext", p->flag_ext, PSA_PLANOGRAM_FLAG_SLOTS);
    json_write_field_int(b, "fill_pattern", p->fill_pattern);
    json_write_field_str(b, "printable_segments", p->printable_segments);
    json_write_field_str(b, "source_file", p->source_file);
    json_write_field_int(b, "changed", p->changed);
    json_write_field_str(b, "layout_file", p->layout_file);
    json_write_field_str(b, "notes", p->notes);
    json_write_field_int64_array(b, "ext_db_keys", p->ext_db_keys, 10);
    json_write_field_int(b, "source_type", p->source_type);
    json_write_field_str_array(b, "status", p->status, 3);
    json_write_field_int_array(b, "date_slots", p->date_slots, 8);
    json_write_field_str(b, "created_by", p->created_by);
    json_write_field_str(b, "modified_by", p->modified_by);
    json_write_field_str(b, "floor_bitmap_ref", p->floor_bitmap_ref);
    json_write_field_double(b, "door_transparency", p->door_transparency);
    json_write_field_double(b, "floor_tile_width", p->floor_tile_width);
    json_write_field_double(b, "floor_tile_depth", p->floor_tile_depth);
    json_write_field_int_array(b, "inv_model_opts", p->inv_model_opts, 6);
    json_write_field_double(b, "case_multiple", p->case_multiple);
    json_write_field_double(b, "days_supply", p->days_supply);
    json_write_field_int(b, "demand_cycle", p->demand_cycle);
    json_write_field_double(b, "peak_safety", p->peak_safety);
    json_write_field_double(b, "backroom_stock", p->backroom_stock);
    json_write_field_double_array(b, "demand", p->demand, 7);
    json_write_field_str(b, "delivery_schedule", p->delivery_schedule);
    json_write_field_str(b, "business_id", p->business_id);
    json_write_field_str(b, "department", p->department);
    json_write_field_str(b, "part_id", p->part_id);
    json_write_field_str(b, "gln_code", p->gln_code);
    json_write_field_str(b, "custom_payload", p->custom_payload);
    json_write_field_str(b, "guid", p->guid);
    json_write_field_str(b, "db_guid", p->db_guid);
    json_write_field_str(b, "abbrev_name", p->abbrev_name);
    json_write_field_str(b, "category", p->category);
    json_write_field_str(b, "subcategory", p->subcategory);
    json_write_field_int(b, "opt_source_code", p->opt_source_code);
    json_write_field_str(b, "allocation_group", p->allocation_group);
    json_write_field_int(b, "allocation_sequence", p->allocation_sequence);
    json_write_field_double(b, "alloc_min_target", p->alloc_min_target);
    json_write_field_double(b, "alloc_max_target", p->alloc_max_target);
    json_write_field_int(b, "split_ctrl", p->split_ctrl);
    json_write_field_int(b, "segment_ctrl", p->segment_ctrl);
    json_write_field_int(b, "status_ctrl", p->status_ctrl);
    json_write_field_int(b, "score_ctrl", p->score_ctrl);
    json_write_field_int(b, "warning_count", p->warning_count);
    json_write_field_int(b, "error_count", p->error_count);
    json_write_field_str(b, "action_text", p->action_text);
    json_write_field_int(b, "stage_limit", p->stage_limit);
    json_write_field_int(b, "type_ref", p->type_ref);
    json_write_field_int(b, "model_ref", p->model_ref);
    json_write_field_int(b, "family_ref", p->family_ref);
    json_write_field_int(b, "version_ref", p->version_ref);
    json_write_field_int(b, "parent_ref", p->parent_ref);
    json_write_field_int(b, "processing_ts", p->processing_ts);
    json_write_field_str(b, "server_text", p->server_text);
    json_write_field_int(b, "final_status", p->final_status);
    json_write_object_end(b);
    return b->err;
}

static int serialize_fixture(json_buf_t *b, const psa_fixture_t *p)
{
    json_write_object_start(b);
    json_write_field_str(b, "type", "Fixture");
    json_write_field_int(b, "planogram_key", p->planogram_key);
    json_write_field_int(b, "type_code", p->type_code);
    json_write_field_str(b, "name", p->name);
    json_write_field_str(b, "key_text", p->key_text);
    json_write_field_double(b, "x", p->x);
    json_write_field_double(b, "width", p->width);
    json_write_field_double(b, "y", p->y);
    json_write_field_double(b, "height", p->height);
    json_write_field_double(b, "z", p->z);
    json_write_field_double(b, "depth", p->depth);
    json_write_field_double(b, "slope", p->slope);
    json_write_field_double(b, "angle", p->angle);
    json_write_field_double(b, "roll", p->roll);
    json_write_field_int(b, "color", p->color);
    json_write_field_str(b, "assembly", p->assembly);
    json_write_field_double_array(b, "fixture_params", p->fixture_params, 9);
    json_write_field_int(b, "collision_fixtures", p->collision_fixtures);
    json_write_field_int(b, "collision_positions", p->collision_positions);
    json_write_field_int(b, "can_obstruct", p->can_obstruct);
    json_write_field_double_array(b, "overhang", p->overhang, 6);
    json_write_field_int(b, "default_merch_style", p->default_merch_style);
    json_write_field_double_array(b, "divider_dim", p->divider_dim, 3);
    json_write_field_int(b, "combinable", p->combinable);
    json_write_field_double_array(b, "grille_notch_peg", p->grille_notch_peg, 7);
    json_write_field_str(b, "primary_label", p->primary_label);
    json_write_field_str(b, "secondary_label", p->secondary_label);
    json_write_field_str(b, "shape_ref", p->shape_ref);
    json_write_field_str(b, "bitmap_ref", p->bitmap_ref);
    json_write_field_int_array(b, "merch_x", p->merch_x, 9);
    json_write_field_int_array(b, "merch_y", p->merch_y, 9);
    json_write_field_int_array(b, "merch_z", p->merch_z, 9);
    json_write_field_str_array(b, "text_ext", p->text_ext, PSA_FIXTURE_TEXT_SLOTS);
    json_write_field_double_array(b, "num_ext", p->num_ext, PSA_FIXTURE_NUM_SLOTS);
    json_write_field_int_array(b, "flag_ext", p->flag_ext, PSA_FIXTURE_FLAG_SLOTS);
    json_write_field_int(b, "location_id", p->location_id);
    json_write_field_int(b, "fill_pattern", p->fill_pattern);
    json_write_field_str(b, "model_file", p->model_file);
    json_write_field_double(b, "weight_capacity", p->weight_capacity);
    json_write_field_int(b, "changed", p->changed);
    json_write_field_int_array(b, "divider_placement", p->divider_placement, 3);
    json_write_field_double(b, "transparency", p->transparency);
    json_write_field_int(b, "hide_when_printing", p->hide_when_printing);
    json_write_field_str(b, "product_assoc", p->product_assoc);
    json_write_field_str(b, "part_id", p->part_id);
    json_write_field_int(b, "hide_view_dims", p->hide_view_dims);
    json_write_field_str(b, "gln_code", p->gln_code);
    json_write_object_end(b);
    return b->err;
}

static int serialize_product(json_buf_t *b, const psa_product_t *p)
{
    json_write_object_start(b);
    json_write_field_str(b, "type", "Product");
    json_write_field_str(b, "upc", p->upc);
    json_write_field_str(b, "business_id", p->business_id);
    json_write_field_str(b, "name", p->name);
    json_write_field_str(b, "key_text", p->key_text);
    json_write_field_double(b, "width", p->width);
    json_write_field_double(b, "height", p->height);
    json_write_field_double(b, "depth", p->depth);
    json_write_field_int(b, "color", p->color);
    json_write_field_str(b, "abbrev_name", p->abbrev_name);
    json_write_field_double(b, "size", p->size);
    json_write_field_str(b, "uom", p->uom);
    json_write_field_str(b, "manufacturer", p->manufacturer);
    json_write_field_str(b, "category", p->category);
    json_write_field_str(b, "supplier", p->supplier);
    json_write_field_int(b, "inner_pack_qty", p->inner_pack_qty);
    json_write_field_double(b, "nesting_x", p->nesting_x);
    json_write_field_double(b, "nesting_y", p->nesting_y);
    json_write_field_double(b, "nesting_z", p->nesting_z);
    json_write_field_int(b, "peg_hole_count", p->peg_hole_count);
    json_write_field_double_array(b, "peg_hole_geom", p->peg_hole_geom, 9);
    json_write_field_int(b, "packaging_style", p->packaging_style);
    json_write_field_str(b, "peg_profile", p->peg_profile);
    json_write_field_double(b, "finger_space_y", p->finger_space_y);
    json_write_field_double(b, "jumble_factor", p->jumble_factor);
    json_write_field_double(b, "price", p->price);
    json_write_field_double(b, "case_cost", p->case_cost);
    json_write_field_int(b, "tax_code", p->tax_code);
    json_write_field_double(b, "unit_movement", p->unit_movement);
    json_write_field_double(b, "share", p->share);
    json_write_field_double(b, "case_multiple", p->case_multiple);
    json_write_field_double(b, "days_supply", p->days_supply);
    json_write_field_double(b, "combined_perf", p->combined_perf);
    json_write_field_int(b, "peg_span", p->peg_span);
    json_write_field_int(b, "min_units", p->min_units);
    json_write_field_int(b, "max_units", p->max_units);
    json_write_field_str(b, "shape_ref", p->shape_ref);
    json_write_field_str(b, "bitmap_ref", p->bitmap_ref);
    json_write_field_double_array(b, "tray", p->tray, 8);
    json_write_field_double_array(b, "case_pack", p->case_pack, 8);
    json_write_field_double_array(b, "display", p->display, 8);
    json_write_field_double_array(b, "alternate", p->alternate, 8);
    json_write_field_double_array(b, "loose", p->loose, 8);
    json_write_field_int_array(b, "merch_xyz", p->merch_xyz, 27);
    json_write_field_int(b, "num_positions", p->num_positions);
    json_write_field_str_array(b, "text_ext", p->text_ext, PSA_PRODUCT_TEXT_SLOTS);
    json_write_field_double_array(b, "num_ext", p->num_ext, PSA_PRODUCT_NUM_SLOTS);
    json_write_field_int_array(b, "flag_ext", p->flag_ext, PSA_PRODUCT_FLAG_SLOTS);
    json_write_field_double(b, "squeeze_min_x", p->squeeze_min_x);
    json_write_field_double(b, "squeeze_max_x", p->squeeze_max_x);
    json_write_field_double(b, "squeeze_min_y", p->squeeze_min_y);
    json_write_field_double(b, "squeeze_max_y", p->squeeze_max_y);
    json_write_field_double(b, "squeeze_min_z", p->squeeze_min_z);
    json_write_field_double(b, "squeeze_max_z", p->squeeze_max_z);
    json_write_field_int(b, "fill_pattern", p->fill_pattern);
    json_write_field_str(b, "model_file", p->model_file);
    json_write_field_str(b, "brand", p->brand);
    json_write_field_str(b, "subcategory", p->subcategory);
    json_write_field_double(b, "weight", p->weight);
    json_write_field_str(b, "planogram_alias", p->planogram_alias);
    json_write_field_int(b, "changed", p->changed);
    json_write_field_double(b, "front_overhang", p->front_overhang);
    json_write_field_double(b, "finger_space_x", p->finger_space_x);
    json_write_field_int64_array(b, "ext_db_keys", p->ext_db_keys, 10);
    json_write_field_str(b, "status", p->status);
    json_write_field_int_array(b, "date_slots", p->date_slots, 8);
    json_write_field_str(b, "created_by", p->created_by);
    json_write_field_str(b, "modified_by", p->modified_by);
    json_write_field_double(b, "transparency", p->transparency);
    json_write_field_double(b, "peak_safety", p->peak_safety);
    json_write_field_double(b, "backroom_stock", p->backroom_stock);
    json_write_field_str(b, "delivery_schedule", p->delivery_schedule);
    json_write_field_str(b, "part_id", p->part_id);
    json_write_field_int(b, "authority_level", p->authority_level);
    json_write_field_int(b, "bitmap_unit_override", p->bitmap_unit_override);
    json_write_field_int(b, "model_lookup_mode", p->model_lookup_mode);
    json_write_field_int(b, "default_merch_style", p->default_merch_style);
    json_write_field_int(b, "auto_model", p->auto_model);
    json_write_field_str(b, "custom_payload", p->custom_payload);
    json_write_field_str(b, "db_guid", p->db_guid);
    json_write_field_int(b, "source_code", p->source_code);
    json_write_field_int64(b, "technical_key", p->technical_key);
    json_write_object_end(b);
    return b->err;
}

static int serialize_position(json_buf_t *b, const psa_position_t *p)
{
    json_write_object_start(b);
    json_write_field_str(b, "type", "Position");
    json_write_field_int(b, "planogram_key", p->planogram_key);
    json_write_field_str(b, "upc", p->upc);
    json_write_field_str(b, "business_id", p->business_id);
    json_write_field_str(b, "key_text", p->key_text);
    json_write_field_double(b, "x", p->x);
    json_write_field_double(b, "width", p->width);
    json_write_field_double(b, "y", p->y);
    json_write_field_double(b, "height", p->height);
    json_write_field_double(b, "z", p->z);
    json_write_field_double(b, "depth", p->depth);
    json_write_field_double(b, "slope", p->slope);
    json_write_field_double(b, "angle", p->angle);
    json_write_field_double(b, "roll", p->roll);
    json_write_field_int(b, "merch_style", p->merch_style);
    json_write_field_int(b, "h_facing", p->h_facing);
    json_write_field_int(b, "v_facing", p->v_facing);
    json_write_field_int(b, "d_facing", p->d_facing);
    json_write_field_int_array(b, "x_cap", p->x_cap, 4);
    json_write_field_int_array(b, "y_cap", p->y_cap, 4);
    json_write_field_int_array(b, "z_cap", p->z_cap, 4);
    json_write_field_int(b, "orientation", p->orientation);
    json_write_field_double(b, "jumble_x", p->jumble_x);
    json_write_field_double(b, "jumble_y", p->jumble_y);
    json_write_field_double(b, "jumble_z", p->jumble_z);
    json_write_field_double(b, "merch_dim_x", p->merch_dim_x);
    json_write_field_double(b, "merch_dim_y", p->merch_dim_y);
    json_write_field_double(b, "merch_dim_z", p->merch_dim_z);
    json_write_field_double(b, "full_dim_x", p->full_dim_x);
    json_write_field_double(b, "full_dim_y", p->full_dim_y);
    json_write_field_double(b, "full_dim_z", p->full_dim_z);
    json_write_field_int(b, "subunit_x", p->subunit_x);
    json_write_field_int(b, "subunit_y", p->subunit_y);
    json_write_field_int(b, "subunit_z", p->subunit_z);
    json_write_field_str(b, "peg_profile", p->peg_profile);
    json_write_field_int(b, "manual_units", p->manual_units);
    json_write_field_int(b, "rank_x", p->rank_x);
    json_write_field_int(b, "rank_y", p->rank_y);
    json_write_field_int(b, "rank_z", p->rank_z);
    json_write_field_int(b, "peg_span", p->peg_span);
    json_write_field_int(b, "always_float", p->always_float);
    json_write_field_str(b, "primary_label", p->primary_label);
    json_write_field_str(b, "secondary_label", p->secondary_label);
    json_write_field_int_array(b, "merch_xyz", p->merch_xyz, 27);
    json_write_field_str_array(b, "text_ext", p->text_ext, PSA_POSITION_TEXT_SLOTS);
    json_write_field_double_array(b, "num_ext", p->num_ext, PSA_POSITION_NUM_SLOTS);
    json_write_field_int_array(b, "flag_ext", p->flag_ext, PSA_POSITION_FLAG_SLOTS);
    json_write_field_int(b, "target_space_x", p->target_space_x);
    json_write_field_int(b, "target_space_y", p->target_space_y);
    json_write_field_int(b, "target_space_z", p->target_space_z);
    json_write_field_double(b, "target_val_x", p->target_val_x);
    json_write_field_double(b, "target_val_y", p->target_val_y);
    json_write_field_double(b, "target_val_z", p->target_val_z);
    json_write_field_int(b, "location_id", p->location_id);
    json_write_field_int(b, "changed", p->changed);
    json_write_field_int(b, "replenishment_min", p->replenishment_min);
    json_write_field_int(b, "replenishment_max", p->replenishment_max);
    json_write_field_str(b, "shape_ref", p->shape_ref);
    json_write_field_str(b, "bitmap_ref", p->bitmap_ref);
    json_write_field_int(b, "hide_when_printing", p->hide_when_printing);
    json_write_field_str(b, "part_id", p->part_id);
    json_write_field_int(b, "bitmap_unit_override", p->bitmap_unit_override);
    json_write_field_int(b, "auto_model", p->auto_model);
    json_write_field_str(b, "custom_payload", p->custom_payload);
    json_write_field_int(b, "x_cap_includes_units", p->x_cap_includes_units);
    json_write_field_int(b, "y_cap_includes_units", p->y_cap_includes_units);
    json_write_object_end(b);
    return b->err;
}

static int serialize_performance(json_buf_t *b, const psa_performance_t *p)
{
    json_write_object_start(b);
    json_write_field_str(b, "type", "Performance");
    json_write_field_str(b, "upc", p->upc);
    json_write_field_str(b, "business_id", p->business_id);
    json_write_field_str(b, "key_text", p->key_text);
    json_write_field_double(b, "price", p->price);
    json_write_field_double(b, "case_cost", p->case_cost);
    json_write_field_int(b, "tax_code", p->tax_code);
    json_write_field_double(b, "unit_movement", p->unit_movement);
    json_write_field_double(b, "share", p->share);
    json_write_field_double(b, "combined_perf", p->combined_perf);
    json_write_field_str_array(b, "text_ext", p->text_ext, PSA_PERF_TEXT_SLOTS);
    json_write_field_double_array(b, "num_ext", p->num_ext, PSA_PERF_NUM_SLOTS);
    json_write_field_int_array(b, "flag_ext", p->flag_ext, PSA_PERF_FLAG_SLOTS);
    json_write_field_int(b, "changed", p->changed);
    json_write_field_double(b, "case_multiple", p->case_multiple);
    json_write_field_double(b, "days_supply", p->days_supply);
    json_write_field_double(b, "peak_safety", p->peak_safety);
    json_write_field_double(b, "backroom_stock", p->backroom_stock);
    json_write_field_int(b, "min_units", p->min_units);
    json_write_field_int(b, "max_units", p->max_units);
    json_write_field_str(b, "delivery_schedule", p->delivery_schedule);
    json_write_field_int(b, "replenishment_min", p->replenishment_min);
    json_write_field_int(b, "replenishment_max", p->replenishment_max);
    json_write_field_int(b, "assortment_rank", p->assortment_rank);
    json_write_field_int(b, "recommended_facings", p->recommended_facings);
    json_write_field_str(b, "assortment_strategy", p->assortment_strategy);
    json_write_field_str(b, "assortment_tactic", p->assortment_tactic);
    json_write_field_str(b, "assortment_reason", p->assortment_reason);
    json_write_field_str(b, "assortment_action", p->assortment_action);
    json_write_field_str(b, "part_id", p->part_id);
    json_write_field_str(b, "cluster_name", p->cluster_name);
    json_write_field_int(b, "target_store_count", p->target_store_count);
    json_write_field_double(b, "target_dist_percent", p->target_dist_percent);
    json_write_field_str(b, "assortment_note", p->assortment_note);
    json_write_field_str(b, "custom_payload", p->custom_payload);
    json_write_field_int(b, "recommended_orientation", p->recommended_orientation);
    json_write_field_int(b, "recommended_merch_style", p->recommended_merch_style);
    json_write_field_int(b, "ignore_recommendations", p->ignore_recommendations);
    json_write_field_int(b, "priority_code", p->priority_code);
    json_write_field_str(b, "priority_desc", p->priority_desc);
    json_write_field_int(b, "force_list", p->force_list);
    json_write_field_str(b, "planogram_reason", p->planogram_reason);
    json_write_field_double(b, "max_stage_reduction", p->max_stage_reduction);
    json_write_object_end(b);
    return b->err;
}

static int serialize_segment(json_buf_t *b, const psa_segment_t *p)
{
    json_write_object_start(b);
    json_write_field_str(b, "type", "Segment");
    json_write_field_int(b, "planogram_key", p->planogram_key);
    json_write_field_str(b, "name", p->name);
    json_write_field_str(b, "key_text", p->key_text);
    json_write_field_double(b, "x", p->x);
    json_write_field_double(b, "width", p->width);
    json_write_field_double(b, "y", p->y);
    json_write_field_double(b, "height", p->height);
    json_write_field_double(b, "z", p->z);
    json_write_field_double(b, "depth", p->depth);
    json_write_field_double(b, "angle", p->angle);
    json_write_field_double(b, "x_offset", p->x_offset);
    json_write_field_double(b, "y_offset", p->y_offset);
    json_write_field_int(b, "door_flag", p->door_flag);
    json_write_field_int(b, "door_direction", p->door_direction);
    json_write_field_str_array(b, "text_ext", p->text_ext, PSA_SEGMENT_TEXT_SLOTS);
    json_write_field_double_array(b, "num_ext", p->num_ext, PSA_SEGMENT_NUM_SLOTS);
    json_write_field_int_array(b, "flag_ext", p->flag_ext, PSA_SEGMENT_FLAG_SLOTS);
    json_write_field_double(b, "frame_width", p->frame_width);
    json_write_field_double(b, "frame_height", p->frame_height);
    json_write_field_int(b, "changed", p->changed);
    json_write_field_int(b, "frame_color", p->frame_color);
    json_write_field_int(b, "frame_fill_pattern", p->frame_fill_pattern);
    json_write_field_str(b, "part_id", p->part_id);
    json_write_field_str(b, "gln_code", p->gln_code);
    json_write_field_str(b, "custom_payload", p->custom_payload);
    json_write_object_end(b);
    return b->err;
}

static int serialize_drawing(json_buf_t *b, const psa_drawing_t *p)
{
    json_write_object_start(b);
    json_write_field_str(b, "type", "Drawing");
    json_write_field_int(b, "drawing_type", p->drawing_type);
    json_write_field_str(b, "name", p->name);
    json_write_field_str(b, "key_text", p->key_text);
    json_write_field_double(b, "x", p->x);
    json_write_field_double(b, "width", p->width);
    json_write_field_double(b, "y", p->y);
    json_write_field_double(b, "height", p->height);
    json_write_field_double(b, "z", p->z);
    json_write_field_double(b, "depth", p->depth);
    json_write_field_int(b, "fg_color", p->fg_color);
    json_write_field_int(b, "bg_fill", p->bg_fill);
    json_write_field_int(b, "bg_color", p->bg_color);
    json_write_field_int(b, "created_in_view", p->created_in_view);
    json_write_field_int(b, "show_in_all_views", p->show_in_all_views);
    json_write_field_int(b, "word_wrap", p->word_wrap);
    json_write_field_int(b, "circular", p->circular);
    json_write_field_double(b, "start_x", p->start_x);
    json_write_field_double(b, "start_y", p->start_y);
    json_write_field_double(b, "start_z", p->start_z);
    json_write_field_double(b, "end_x", p->end_x);
    json_write_field_double(b, "end_y", p->end_y);
    json_write_field_double(b, "end_z", p->end_z);
    json_write_field_str(b, "text", p->text);
    json_write_field_int(b, "text_scale", p->text_scale);
    json_write_field_int(b, "outline", p->outline);
    json_write_field_int(b, "callout", p->callout);
    json_write_field_int64_array(b, "font_metrics", p->font_metrics, 5);
    json_write_field_int_array(b, "font_style", p->font_style, 8);
    json_write_field_str(b, "font_face", p->font_face);
    json_write_field_double(b, "anchor_x", p->anchor_x);
    json_write_field_double(b, "anchor_y", p->anchor_y);
    json_write_field_double(b, "anchor_z", p->anchor_z);
    json_write_field_int(b, "center_text", p->center_text);
    json_write_field_int(b, "changed", p->changed);
    json_write_field_int(b, "hide_when_printing", p->hide_when_printing);
    json_write_field_str(b, "custom_payload", p->custom_payload);
    json_write_object_end(b);
    return b->err;
}

static int serialize_divider(json_buf_t *b, const psa_divider_t *p)
{
    json_write_object_start(b);
    json_write_field_str(b, "type", "Divider");
    json_write_field_str(b, "id", p->id);
    json_write_field_double(b, "x", p->x);
    json_write_field_double(b, "width", p->width);
    json_write_field_double(b, "y", p->y);
    json_write_field_double(b, "height", p->height);
    json_write_field_double(b, "z", p->z);
    json_write_field_double(b, "depth", p->depth);
    json_write_field_int(b, "color", p->color);
    json_write_field_str(b, "undef_text_1", p->undef_text_1);
    json_write_field_str(b, "desc_text_1", p->desc_text_1);
    json_write_field_str(b, "desc_text_2", p->desc_text_2);
    json_write_field_str(b, "desc_text_3", p->desc_text_3);
    json_write_field_double(b, "num_1", p->num_1);
    json_write_field_double(b, "num_2", p->num_2);
    json_write_field_double(b, "num_3", p->num_3);
    json_write_field_str(b, "undef_text_2", p->undef_text_2);
    json_write_field_str(b, "undef_text_3", p->undef_text_3);
    json_write_object_end(b);
    return b->err;
}

/* ======================================================================== */
/* Finalize and public single-record API                                    */
/* ======================================================================== */

static int finalize_json(json_buf_t *b, char *out, size_t out_size,
                          size_t *out_written, size_t *out_needed)
{
    if (out_needed)
        *out_needed = b->len + 1;

    if (out && out_size > 0) {
        size_t idx = 0;
        if (b->len < out_size)
            idx = b->len;
        else
            idx = out_size - 1;
        out[idx] = '\0';
    }

    if (b->err != PSA_OK)
        return b->err;

    if (out_written)
        *out_written = b->len;
    return PSA_OK;
}

int psa_file_meta_to_json(const char *header, const char *version,
                          char *out, size_t out_size,
                          size_t *out_written,
                          size_t *out_needed)
{
    if (out_size > 0 && !out)
        return PSA_ERR_INVALID_ARG;

    json_buf_t b = { out, out_size, 0, PSA_OK, 0 };
    json_write_object_start(&b);
    json_write_field_str(&b, "header", header ? header : "");
    json_write_field_str(&b, "version", version ? version : "");
    json_write_object_end(&b);

    return finalize_json(&b, out, out_size, out_written, out_needed);
}

int psa_record_to_json(const psa_record_t *rec,
                       char *out, size_t out_size,
                       size_t *out_written,
                       size_t *out_needed)
{
    if (!rec || (out_size > 0 && !out))
        return PSA_ERR_INVALID_ARG;

    json_buf_t b = { out, out_size, 0, PSA_OK, 0 };
    int rc;

    switch (rec->type) {
        case PSA_REC_PROJECT:     rc = serialize_project(&b, &rec->rec.project);     break;
        case PSA_REC_PLANOGRAM:   rc = serialize_planogram(&b, &rec->rec.planogram); break;
        case PSA_REC_FIXTURE:     rc = serialize_fixture(&b, &rec->rec.fixture);     break;
        case PSA_REC_PRODUCT:     rc = serialize_product(&b, &rec->rec.product);     break;
        case PSA_REC_POSITION:    rc = serialize_position(&b, &rec->rec.position);   break;
        case PSA_REC_PERFORMANCE: rc = serialize_performance(&b, &rec->rec.performance); break;
        case PSA_REC_SEGMENT:     rc = serialize_segment(&b, &rec->rec.segment);     break;
        case PSA_REC_DRAWING:     rc = serialize_drawing(&b, &rec->rec.drawing);     break;
        case PSA_REC_DIVIDER:     rc = serialize_divider(&b, &rec->rec.divider);     break;
        default: return PSA_ERR_INVALID_ARG;
    }

    (void)rc; /* b->err already carries the result */
    return finalize_json(&b, out, out_size, out_written, out_needed);
}

/* ======================================================================== */
/* Document accumulation                                                    */
/* ======================================================================== */

typedef struct {
    char   *buf;
    size_t  len;
    size_t  cap;
    int     count;
} json_bucket_t;

typedef struct {
    json_bucket_t buckets[9];
    int           failed_rc;
    size_t        total_bytes;
    size_t        max_document_bytes;
} doc_accum_t;

static int bucket_append(doc_accum_t *acc, json_bucket_t *b, const char *s, size_t n)
{
    if (acc->total_bytes > SIZE_MAX - n)
        return PSA_ERR_OVERFLOW;
    if (acc->max_document_bytes > 0 && acc->total_bytes + n > acc->max_document_bytes)
        return PSA_ERR_OVERFLOW;

    size_t need = b->len + n + 1;
    if (need > b->cap) {
        size_t new_cap = b->cap ? b->cap : 1024;
        while (new_cap < need) {
            if (new_cap > SIZE_MAX / 2)
                return PSA_ERR_OVERFLOW;
            new_cap *= 2;
        }
        char *new_buf = realloc(b->buf, new_cap);
        if (!new_buf)
            return PSA_ERR_NOMEM;
        b->buf = new_buf;
        b->cap = new_cap;
    }

    memcpy(b->buf + b->len, s, n);
    b->len += n;
    b->buf[b->len] = '\0';
    acc->total_bytes += n;
    return PSA_OK;
}

static int document_cb(psa_record_t *rec, void *user_data)
{
    doc_accum_t *acc = (doc_accum_t *)user_data;
    if ((int)rec->type < 0 || (int)rec->type >= 9) {
        acc->failed_rc = PSA_ERR_INVALID_ARG;
        return 1;
    }

    size_t needed = 0;
    int rc = psa_record_to_json(rec, NULL, 0, NULL, &needed);
    if (rc != PSA_OK) {
        acc->failed_rc = rc;
        return 1;
    }

    char *tmp = malloc(needed);
    if (!tmp) {
        acc->failed_rc = PSA_ERR_NOMEM;
        return 1;
    }

    size_t written = 0;
    rc = psa_record_to_json(rec, tmp, needed, &written, NULL);
    if (rc != PSA_OK) {
        free(tmp);
        acc->failed_rc = rc;
        return 1;
    }

    json_bucket_t *bucket = &acc->buckets[(int)rec->type];
    if (bucket->count > 0) {
        rc = bucket_append(acc, bucket, ",", 1);
        if (rc != PSA_OK) {
            free(tmp);
            acc->failed_rc = rc;
            return 1;
        }
    }

    rc = bucket_append(acc, bucket, tmp, written);
    free(tmp);
    if (rc != PSA_OK) {
        acc->failed_rc = rc;
        return 1;
    }

    bucket->count++;
    return 0;
}

/* ======================================================================== */
/* Shared document emission                                                 */
/* ======================================================================== */

static const char *record_array_names[9] = {
    "projects", "planograms", "fixtures", "products",
    "positions", "performances", "segments", "drawings", "dividers"
};

static int build_json_document(json_buf_t *b, doc_accum_t *acc,
                                const char *header, const char *version)
{
    json_write_object_start(b);
    json_write_field_str(b, "header", header ? header : "");
    json_write_field_str(b, "version", version ? version : "");

    for (int i = 0; i < 9; i++) {
        json_write_key(b, record_array_names[i]);
        json_buf_append_char(b, '[');
        if (acc->buckets[i].len > 0)
            json_buf_append_bytes(b, acc->buckets[i].buf, acc->buckets[i].len);
        json_buf_append_char(b, ']');
    }

    json_write_object_end(b);
    return b->err;
}

static void free_buckets_and_headers(doc_accum_t *acc,
                                       const char **header,
                                       const char **version)
{
    free((void *)*header);
    free((void *)*version);
    *header = NULL;
    *version = NULL;
    for (int i = 0; i < 9; i++)
        free(acc->buckets[i].buf);
}

/* ======================================================================== */
/* Public document API                                                      */
/* ======================================================================== */

int psa_parse_file_to_json_document(const char *path,
                                    char *out, size_t out_size,
                                    size_t *out_written,
                                    size_t *out_needed,
                                    char *errbuf, size_t errbuf_size)
{
    if (!path || (out_size > 0 && !out) || (errbuf_size > 0 && !errbuf)) {
        set_err(errbuf, errbuf_size, "Invalid arguments");
        return PSA_ERR_INVALID_ARG;
    }

    if (errbuf && errbuf_size > 0)
        errbuf[0] = '\0';

    doc_accum_t acc = {0};
    const char *header = NULL;
    const char *version = NULL;
    int rc;
    json_buf_t b = { out, out_size, 0, PSA_OK, 0 };

    acc.max_document_bytes = PSA_DEFAULT_MAX_DOCUMENT_BYTES;

    psa_parse_limits_t limits = {0};
    limits.max_document_bytes = PSA_DEFAULT_MAX_DOCUMENT_BYTES;

    rc = psa_parse_file_ex(path, &limits, &header, &version,
                           document_cb, &acc, errbuf, errbuf_size);
    if (rc != PSA_OK) {
        if (rc == PSA_ERR_ABORT && acc.failed_rc != 0)
            rc = acc.failed_rc;
        if (rc == PSA_ERR_ABORT && acc.failed_rc == 0 &&
            errbuf && errbuf_size > 0 && errbuf[0] == '\0')
            snprintf(errbuf, errbuf_size, "Failed to build JSON document");
        set_err_for_rc_if_empty(rc, errbuf, errbuf_size);
        free_buckets_and_headers(&acc, &header, &version);
        return rc;
    }

    build_json_document(&b, &acc, header, version);

    free_buckets_and_headers(&acc, &header, &version);

    rc = finalize_json(&b, out, out_size, out_written, out_needed);
    if (rc != PSA_OK)
        set_err_for_rc_if_empty(rc, errbuf, errbuf_size);
    return rc;
}

int psa_parse_buffer_to_json_document(const char *data, size_t data_len,
                                      char *out, size_t out_size,
                                      size_t *out_written,
                                      size_t *out_needed,
                                      char *errbuf, size_t errbuf_size)
{
    if (((!data) && data_len > 0) || (out_size > 0 && !out) ||
        (errbuf_size > 0 && !errbuf)) {
        set_err(errbuf, errbuf_size, "Invalid arguments");
        return PSA_ERR_INVALID_ARG;
    }

    if (errbuf && errbuf_size > 0)
        errbuf[0] = '\0';

    doc_accum_t acc = {0};
    const char *header = NULL;
    const char *version = NULL;
    int rc;
    json_buf_t b = { out, out_size, 0, PSA_OK, 0 };

    acc.max_document_bytes = PSA_DEFAULT_MAX_DOCUMENT_BYTES;

    psa_parse_limits_t limits = {0};
    limits.max_document_bytes = PSA_DEFAULT_MAX_DOCUMENT_BYTES;

    rc = psa_parse_buffer_ex(data, data_len, &limits,
                             &header, &version,
                             document_cb, &acc,
                             errbuf, errbuf_size);
    if (rc != PSA_OK) {
        if (rc == PSA_ERR_ABORT && acc.failed_rc != 0)
            rc = acc.failed_rc;
        if (rc == PSA_ERR_ABORT && acc.failed_rc == 0 &&
            errbuf && errbuf_size > 0 && errbuf[0] == '\0')
            snprintf(errbuf, errbuf_size, "Failed to build JSON document");
        set_err_for_rc_if_empty(rc, errbuf, errbuf_size);
        free_buckets_and_headers(&acc, &header, &version);
        return rc;
    }

    build_json_document(&b, &acc, header, version);

    free_buckets_and_headers(&acc, &header, &version);

    rc = finalize_json(&b, out, out_size, out_written, out_needed);
    if (rc != PSA_OK)
        set_err_for_rc_if_empty(rc, errbuf, errbuf_size);
    return rc;
}
