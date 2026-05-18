#include "psa.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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

/* -------------------------------------------------------------------------- */
/* Helpers to populate each record type with representative data             */
/* -------------------------------------------------------------------------- */

static void populate_project(psa_project_t *p)
{
    memset(p, 0, sizeof(*p));
    p->display_name = "My Project";
    p->key_text = "PROJ-001";
    p->primary_key = 42;
    p->layout_file = "layout.psa";
    p->movement_period = 7;
    p->case_multiple = 1.5;
    p->days_supply = 14.0;
    p->demand_cycle = 30;
    p->peak_safety = 0.25;
    p->backroom_stock = 100.0;
    p->peg_profile = "standard";
    p->measurement_mode = 1;
    p->num_stores = 500;
    for (int i = 0; i < 9; i++) {
        p->merch_x[i] = i;
        p->merch_y[i] = i * 10;
        p->merch_z[i] = i * 100;
    }
    p->demand[0] = 1.1; p->demand[1] = 2.2; p->demand[2] = 3.3;
    p->inv_model_opts[0] = 1; p->inv_model_opts[1] = 2;
    p->num_ext[0] = 9.9;
    p->text_ext[0] = "ext1";
    p->flag_ext[0] = 1;
    p->notes = "some notes";
    p->changed = 1;
    p->ext_db_keys[0] = INT64_C(9223372036854775807);
    p->ext_db_keys[1] = INT64_C(-9223372036854775807) - 1;
    p->perf_override[0] = 1; p->perf_override[1] = 0; p->perf_override[2] = 1; p->perf_override[3] = 0;
    p->status = "active";
    p->date_slots[0] = 20240101;
    p->created_by = "alice";
    p->modified_by = "bob";
    p->inv_mode = 2;
    p->delivery_schedule = "weekly";
    p->custom_payload = "{\"key\":\"val\"}";
    p->family_key = 99;
}

static void populate_planogram(psa_planogram_t *p)
{
    memset(p, 0, sizeof(*p));
    p->name = "Planogram A";
    p->key_text = "PLANO-001";
    p->width = 120.5;
    p->height = 200.0;
    p->depth = 60.25;
    p->display_color = 16711680;
    p->back_depth = 15.0;
    p->draw_back = 1;
    p->base_width = 10.0;
    p->base_height = 5.0;
    p->base_depth = 10.0;
    p->draw_base = 0;
    p->base_color = 0;
    p->notch_peg[0] = 1.0; p->notch_peg[1] = 2.0;
    p->traffic_flow = 1;
    p->auto_created = 0;
    p->shape_ref = "shape1";
    p->bitmap_ref = "bitmap1";
    for (int i = 0; i < 9; i++) {
        p->merch_x[i] = i;
        p->merch_y[i] = i + 1;
        p->merch_z[i] = i + 2;
    }
    p->combined_perf = 0.85;
    p->store_count = 50;
    p->notch_width = 2.5;
    p->text_ext[0] = "te1";
    p->num_ext[0] = 7.7;
    p->flag_ext[0] = 1;
    p->fill_pattern = 3;
    p->printable_segments = "seg1,seg2";
    p->source_file = "source.psa";
    p->changed = 1;
    p->layout_file = "layout.psa";
    p->notes = "notes here";
    p->ext_db_keys[0] = INT64_C(1234567890);
    p->source_type = 1;
    p->status[0] = "ok"; p->status[1] = "review"; p->status[2] = "done";
    p->date_slots[0] = 20230101;
    p->created_by = "alice";
    p->modified_by = "bob";
    p->floor_bitmap_ref = "floor.bmp";
    p->door_transparency = 0.5;
    p->floor_tile_width = 12.0;
    p->floor_tile_depth = 12.0;
    p->inv_model_opts[0] = 1; p->inv_model_opts[1] = 2;
    p->case_multiple = 2.0;
    p->days_supply = 7.0;
    p->demand_cycle = 14;
    p->peak_safety = 0.1;
    p->backroom_stock = 50.0;
    p->demand[0] = 10.0; p->demand[1] = 20.0;
    p->delivery_schedule = "daily";
    p->business_id = "BIZ-001";
    p->department = "Dept A";
    p->part_id = "PART-001";
    p->gln_code = "GLN123";
    p->custom_payload = "{}";
    p->guid = "guid-1";
    p->db_guid = "db-guid-1";
    p->abbrev_name = "PA";
    p->category = "Cat1";
    p->subcategory = "Sub1";
    p->opt_source_code = 1;
    p->allocation_group = "AG1";
    p->allocation_sequence = 5;
    p->alloc_min_target = 0.8;
    p->alloc_max_target = 1.2;
    p->split_ctrl = 1;
    p->segment_ctrl = 2;
    p->status_ctrl = 3;
    p->score_ctrl = 4;
    p->warning_count = 0;
    p->error_count = 0;
    p->action_text = "none";
    p->stage_limit = 10;
    p->type_ref = 1;
    p->model_ref = 2;
    p->family_ref = 3;
    p->version_ref = 4;
    p->parent_ref = 5;
    p->processing_ts = 1234567890;
    p->server_text = "srv1";
    p->final_status = 1;
}

static void populate_fixture(psa_fixture_t *p)
{
    memset(p, 0, sizeof(*p));
    p->planogram_key = 7;
    p->type_code = 1;
    p->name = "Shelf";
    p->key_text = "FIX-001";
    p->x = 10.0; p->width = 120.0; p->y = 0.0; p->height = 200.0; p->z = 0.0; p->depth = 60.0;
    p->slope = 0.0; p->angle = 0.0; p->roll = 0.0;
    p->color = 0;
    p->assembly = "std";
    p->fixture_params[0] = 1.0; p->fixture_params[1] = 2.0;
    p->collision_fixtures = 1;
    p->collision_positions = 0;
    p->can_obstruct = 1;
    p->overhang[0] = 0.0; p->overhang[1] = 0.0;
    p->default_merch_style = 1;
    p->divider_dim[0] = 1.0; p->divider_dim[1] = 2.0; p->divider_dim[2] = 3.0;
    p->combinable = 0;
    p->grille_notch_peg[0] = 1.0;
    p->primary_label = "label1";
    p->secondary_label = "label2";
    p->shape_ref = "sref";
    p->bitmap_ref = "bref";
    for (int i = 0; i < 9; i++) {
        p->merch_x[i] = i;
        p->merch_y[i] = i;
        p->merch_z[i] = i;
    }
    p->text_ext[0] = "fte1";
    p->num_ext[0] = 1.1;
    p->flag_ext[0] = 1;
    p->location_id = 5;
    p->fill_pattern = 1;
    p->model_file = "model.obj";
    p->weight_capacity = 50.0;
    p->changed = 1;
    p->divider_placement[0] = 1; p->divider_placement[1] = 0; p->divider_placement[2] = 0;
    p->transparency = 0.0;
    p->hide_when_printing = 0;
    p->product_assoc = "PROD-001";
    p->part_id = "FPART-1";
    p->hide_view_dims = 0;
    p->gln_code = "GLN-FIX";
}

static void populate_product(psa_product_t *p)
{
    memset(p, 0, sizeof(*p));
    p->upc = "123456789012";
    p->business_id = "BIZ-PROD";
    p->name = "Widget";
    p->key_text = "PROD-001";
    p->width = 5.0; p->height = 10.0; p->depth = 3.0;
    p->color = 255;
    p->abbrev_name = "W";
    p->size = 1.0;
    p->uom = "ea";
    p->manufacturer = "Acme";
    p->category = "Tools";
    p->supplier = "Supp1";
    p->inner_pack_qty = 6;
    p->nesting_x = 0.0; p->nesting_y = 0.0; p->nesting_z = 0.0;
    p->peg_hole_count = 2;
    p->peg_hole_geom[0] = 1.0; p->peg_hole_geom[1] = 2.0;
    p->packaging_style = 1;
    p->peg_profile = "std";
    p->finger_space_y = 0.5;
    p->jumble_factor = 0.0;
    p->price = 9.99;
    p->case_cost = 5.00;
    p->tax_code = 1;
    p->unit_movement = 100.0;
    p->share = 0.1;
    p->case_multiple = 2.0;
    p->days_supply = 7.0;
    p->combined_perf = 0.5;
    p->peg_span = 1;
    p->min_units = 1;
    p->max_units = 10;
    p->shape_ref = "pshape";
    p->bitmap_ref = "pbmp";
    p->tray[0] = 1.0; p->tray[1] = 2.0;
    p->case_pack[0] = 6.0;
    p->display[0] = 1.0;
    p->alternate[0] = 0.0;
    p->loose[0] = 1.0;
    for (int i = 0; i < 27; i++)
        p->merch_xyz[i] = i;
    p->num_positions = 1;
    p->text_ext[0] = "pte1";
    p->num_ext[0] = 3.14;
    p->flag_ext[0] = 1;
    p->squeeze_min_x = 4.0; p->squeeze_max_x = 6.0;
    p->squeeze_min_y = 4.0; p->squeeze_max_y = 6.0;
    p->squeeze_min_z = 4.0; p->squeeze_max_z = 6.0;
    p->fill_pattern = 1;
    p->model_file = "pmodel.obj";
    p->brand = "AcmeBrand";
    p->subcategory = "Sub";
    p->weight = 0.5;
    p->planogram_alias = "alias1";
    p->changed = 1;
    p->front_overhang = 0.0;
    p->finger_space_x = 0.0;
    p->ext_db_keys[0] = INT64_C(9876543210);
    p->status = "active";
    p->date_slots[0] = 20240101;
    p->created_by = "alice";
    p->modified_by = "bob";
    p->transparency = 0.0;
    p->peak_safety = 0.1;
    p->backroom_stock = 20.0;
    p->delivery_schedule = "weekly";
    p->part_id = "PPART-1";
    p->authority_level = 1;
    p->bitmap_unit_override = 0;
    p->model_lookup_mode = 1;
    p->default_merch_style = 1;
    p->auto_model = 0;
    p->custom_payload = "{}";
    p->db_guid = "db-prod-1";
    p->source_code = 1;
    p->technical_key = INT64_C(5555555555);
}

static void populate_position(psa_position_t *p)
{
    memset(p, 0, sizeof(*p));
    p->planogram_key = 7;
    p->upc = "123456789012";
    p->business_id = "BIZ-POS";
    p->key_text = "POS-001";
    p->x = 10.0; p->width = 5.0; p->y = 20.0; p->height = 10.0; p->z = 0.0; p->depth = 3.0;
    p->slope = 0.0; p->angle = 0.0; p->roll = 0.0;
    p->merch_style = 1;
    p->h_facing = 1; p->v_facing = 1; p->d_facing = 1;
    p->x_cap[0] = 2; p->x_cap[1] = 0; p->x_cap[2] = 0; p->x_cap[3] = 0;
    p->y_cap[0] = 1; p->y_cap[1] = 0; p->y_cap[2] = 0; p->y_cap[3] = 0;
    p->z_cap[0] = 1; p->z_cap[1] = 0; p->z_cap[2] = 0; p->z_cap[3] = 0;
    p->orientation = 2;
    p->jumble_x = 0.0; p->jumble_y = 0.0; p->jumble_z = 0.0;
    p->merch_dim_x = 5.0; p->merch_dim_y = 10.0; p->merch_dim_z = 3.0;
    p->full_dim_x = 5.0; p->full_dim_y = 10.0; p->full_dim_z = 3.0;
    p->subunit_x = 1; p->subunit_y = 1; p->subunit_z = 1;
    p->peg_profile = "std";
    p->manual_units = 0;
    p->rank_x = 1; p->rank_y = 1; p->rank_z = 1;
    p->peg_span = 1;
    p->always_float = 0;
    p->primary_label = "plab";
    p->secondary_label = "slab";
    for (int i = 0; i < 27; i++)
        p->merch_xyz[i] = i;
    p->text_ext[0] = "xte1";
    p->num_ext[0] = 2.71;
    p->flag_ext[0] = 1;
    p->target_space_x = 0; p->target_space_y = 0; p->target_space_z = 0;
    p->target_val_x = 0.0; p->target_val_y = 0.0; p->target_val_z = 0.0;
    p->location_id = 5;
    p->changed = 1;
    p->replenishment_min = 0; p->replenishment_max = 10;
    p->shape_ref = "pshape";
    p->bitmap_ref = "pbmp";
    p->hide_when_printing = 0;
    p->part_id = "POSPART-1";
    p->bitmap_unit_override = 0;
    p->auto_model = 0;
    p->custom_payload = "{}";
    p->x_cap_includes_units = 0;
    p->y_cap_includes_units = 0;
}

static void populate_performance(psa_performance_t *p)
{
    memset(p, 0, sizeof(*p));
    p->upc = "123456789012";
    p->business_id = "BIZ-PERF";
    p->key_text = "PERF-001";
    p->price = 9.99;
    p->case_cost = 5.00;
    p->tax_code = 1;
    p->unit_movement = 100.0;
    p->share = 0.1;
    p->combined_perf = 0.75;
    p->text_ext[0] = "perfext1";
    p->num_ext[0] = 1.41;
    p->flag_ext[0] = 1;
    p->changed = 1;
    p->case_multiple = 2.0;
    p->days_supply = 7.0;
    p->peak_safety = 0.1;
    p->backroom_stock = 20.0;
    p->min_units = 1;
    p->max_units = 10;
    p->delivery_schedule = "weekly";
    p->replenishment_min = 0;
    p->replenishment_max = 10;
    p->assortment_rank = 1;
    p->recommended_facings = 2;
    p->assortment_strategy = "strat1";
    p->assortment_tactic = "tact1";
    p->assortment_reason = "reason1";
    p->assortment_action = "action1";
    p->part_id = "PERFPART-1";
    p->cluster_name = "cluster1";
    p->target_store_count = 50;
    p->target_dist_percent = 0.8;
    p->assortment_note = "note1";
    p->custom_payload = "{}";
    p->recommended_orientation = 2;
    p->recommended_merch_style = 1;
    p->ignore_recommendations = 0;
    p->priority_code = 1;
    p->priority_desc = "high";
    p->force_list = 0;
    p->planogram_reason = "reason";
    p->max_stage_reduction = 0.1;
}

static void populate_segment(psa_segment_t *p)
{
    memset(p, 0, sizeof(*p));
    p->planogram_key = 7;
    p->name = "Segment A";
    p->key_text = "SEG-001";
    p->x = 0.0; p->width = 120.0; p->y = 0.0; p->height = 200.0; p->z = 0.0; p->depth = 60.0;
    p->angle = 0.0;
    p->x_offset = 0.0; p->y_offset = 0.0;
    p->door_flag = 0;
    p->door_direction = 1;
    p->text_ext[0] = "segext1";
    p->num_ext[0] = 1.0;
    p->flag_ext[0] = 1;
    p->frame_width = 1.0; p->frame_height = 2.0;
    p->changed = 1;
    p->frame_color = 0;
    p->frame_fill_pattern = 1;
    p->part_id = "SEGPART-1";
    p->gln_code = "GLN-SEG";
    p->custom_payload = "{}";
}

static void populate_drawing(psa_drawing_t *p)
{
    memset(p, 0, sizeof(*p));
    p->drawing_type = 1;
    p->name = "Drawing A";
    p->key_text = "DRAW-001";
    p->x = 10.0; p->width = 50.0; p->y = 20.0; p->height = 30.0; p->z = 0.0; p->depth = 0.0;
    p->fg_color = 0;
    p->bg_fill = 1;
    p->bg_color = 16777215;
    p->created_in_view = 1;
    p->show_in_all_views = 1;
    p->word_wrap = 0;
    p->circular = 0;
    p->start_x = 0.0; p->start_y = 0.0; p->start_z = 0.0;
    p->end_x = 10.0; p->end_y = 10.0; p->end_z = 0.0;
    p->text = "Hello\nWorld";
    p->text_scale = 100;
    p->outline = 1;
    p->callout = 0;
    p->font_metrics[0] = INT64_C(1000000000);
    p->font_style[0] = 1; p->font_style[1] = 0;
    p->font_face = "Arial";
    p->anchor_x = 0.0; p->anchor_y = 0.0; p->anchor_z = 0.0;
    p->center_text = 1;
    p->changed = 1;
    p->hide_when_printing = 0;
    p->custom_payload = "{}";
}

static void populate_divider(psa_divider_t *p)
{
    memset(p, 0, sizeof(*p));
    p->id = "DIV-001";
    p->x = 0.0; p->width = 1.0; p->y = 0.0; p->height = 200.0; p->z = 0.0; p->depth = 60.0;
    p->color = 0;
    p->undef_text_1 = "ut1";
    p->desc_text_1 = "dt1";
    p->desc_text_2 = "dt2";
    p->desc_text_3 = "dt3";
    p->num_1 = 1.0; p->num_2 = 2.0; p->num_3 = 3.0;
    p->undef_text_2 = "ut2";
    p->undef_text_3 = "ut3";
}

/* -------------------------------------------------------------------------- */
/* Record-type JSON tests                                                     */
/* -------------------------------------------------------------------------- */

static int test_project_json(void)
{
    psa_record_t rec;
    char out[4096];
    size_t n, need;
    int rc;

    memset(&rec, 0, sizeof(rec));
    rec.type = PSA_REC_PROJECT;
    populate_project(&rec.rec.project);

    rc = psa_record_to_json(&rec, out, sizeof(out), &n, &need);
    FAIL_IF(rc != PSA_OK, "project json failed: %d", rc);
    FAIL_IF(strstr(out, "\"type\":\"Project\"") == NULL, "missing type: %s", out);
    FAIL_IF(strstr(out, "\"display_name\":\"My Project\"") == NULL, "missing display_name: %s", out);
    FAIL_IF(strstr(out, "\"primary_key\":42") == NULL, "missing primary_key: %s", out);
    FAIL_IF(strstr(out, "\"case_multiple\":1.5") == NULL, "missing case_multiple: %s", out);
    FAIL_IF(strstr(out, "\"merch_x\":[0,1,2,3,4,5,6,7,8]") == NULL, "missing merch_x: %s", out);
    FAIL_IF(strstr(out, "\"merch_y\":[0,10,20,30,40,50,60,70,80]") == NULL, "missing merch_y: %s", out);
    FAIL_IF(strstr(out, "\"demand\":[1.1,2.2,3.3") == NULL, "missing demand: %s", out);
    FAIL_IF(strstr(out, "\"inv_model_opts\":[1,2,0,0,0,0]") == NULL, "missing inv_model_opts: %s", out);
    FAIL_IF(strstr(out, "\"ext_db_keys\":[9223372036854775807,-9223372036854775808") == NULL, "missing ext_db_keys: %s", out);
    FAIL_IF(strstr(out, "\"status\":\"active\"") == NULL, "missing status: %s", out);
    FAIL_IF(strstr(out, "\"family_key\":99") == NULL, "missing family_key: %s", out);
    return 0;
}

static int test_planogram_json(void)
{
    psa_record_t rec;
    char out[8192];
    size_t n, need;
    int rc;

    memset(&rec, 0, sizeof(rec));
    rec.type = PSA_REC_PLANOGRAM;
    populate_planogram(&rec.rec.planogram);

    rc = psa_record_to_json(&rec, out, sizeof(out), &n, &need);
    FAIL_IF(rc != PSA_OK, "planogram json failed: %d", rc);
    FAIL_IF(strstr(out, "\"type\":\"Planogram\"") == NULL, "missing type: %s", out);
    FAIL_IF(strstr(out, "\"name\":\"Planogram A\"") == NULL, "missing name: %s", out);
    FAIL_IF(strstr(out, "\"width\":120.5") == NULL, "missing width: %s", out);
    FAIL_IF(strstr(out, "\"notch_peg\":[1,2") == NULL, "missing notch_peg: %s", out);
    FAIL_IF(strstr(out, "\"status\":[\"ok\",\"review\",\"done\"") == NULL, "missing status array: %s", out);
    FAIL_IF(strstr(out, "\"opt_source_code\":1") == NULL, "missing opt_source_code: %s", out);
    FAIL_IF(strstr(out, "\"alloc_min_target\":0.8") == NULL, "missing alloc_min_target: %s", out);
    FAIL_IF(strstr(out, "\"processing_ts\":1234567890") == NULL, "missing processing_ts: %s", out);
    FAIL_IF(strstr(out, "\"final_status\":1") == NULL, "missing final_status: %s", out);
    return 0;
}

static int test_fixture_json(void)
{
    psa_record_t rec;
    char out[4096];
    size_t n, need;
    int rc;

    memset(&rec, 0, sizeof(rec));
    rec.type = PSA_REC_FIXTURE;
    populate_fixture(&rec.rec.fixture);

    rc = psa_record_to_json(&rec, out, sizeof(out), &n, &need);
    FAIL_IF(rc != PSA_OK, "fixture json failed: %d", rc);
    FAIL_IF(strstr(out, "\"type\":\"Fixture\"") == NULL, "missing type: %s", out);
    FAIL_IF(strstr(out, "\"planogram_key\":7") == NULL, "missing planogram_key: %s", out);
    FAIL_IF(strstr(out, "\"name\":\"Shelf\"") == NULL, "missing name: %s", out);
    FAIL_IF(strstr(out, "\"fixture_params\":[1,2") == NULL, "missing fixture_params: %s", out);
    FAIL_IF(strstr(out, "\"overhang\":[0,0") == NULL, "missing overhang: %s", out);
    FAIL_IF(strstr(out, "\"divider_dim\":[1,2,3]") == NULL, "missing divider_dim: %s", out);
    FAIL_IF(strstr(out, "\"grille_notch_peg\":[1") == NULL, "missing grille_notch_peg: %s", out);
    return 0;
}

static int test_product_json(void)
{
    psa_record_t rec;
    char out[8192];
    size_t n, need;
    int rc;

    memset(&rec, 0, sizeof(rec));
    rec.type = PSA_REC_PRODUCT;
    populate_product(&rec.rec.product);

    rc = psa_record_to_json(&rec, out, sizeof(out), &n, &need);
    FAIL_IF(rc != PSA_OK, "product json failed: %d", rc);
    FAIL_IF(strstr(out, "\"type\":\"Product\"") == NULL, "missing type: %s", out);
    FAIL_IF(strstr(out, "\"upc\":\"123456789012\"") == NULL, "missing upc: %s", out);
    FAIL_IF(strstr(out, "\"price\":9.99") == NULL, "missing price: %s", out);
    FAIL_IF(strstr(out, "\"peg_hole_geom\":[1,2") == NULL, "missing peg_hole_geom: %s", out);
    FAIL_IF(strstr(out, "\"tray\":[1,2") == NULL, "missing tray: %s", out);
    FAIL_IF(strstr(out, "\"merch_xyz\":[0,1,2") == NULL, "missing merch_xyz: %s", out);
    FAIL_IF(strstr(out, "\"squeeze_min_x\":4") == NULL, "missing squeeze_min_x: %s", out);
    FAIL_IF(strstr(out, "\"technical_key\":5555555555") == NULL, "missing technical_key: %s", out);
    return 0;
}

static int test_position_json(void)
{
    psa_record_t rec;
    char out[8192];
    size_t n, need;
    int rc;

    memset(&rec, 0, sizeof(rec));
    rec.type = PSA_REC_POSITION;
    populate_position(&rec.rec.position);

    rc = psa_record_to_json(&rec, out, sizeof(out), &n, &need);
    FAIL_IF(rc != PSA_OK, "position json failed: %d", rc);
    FAIL_IF(strstr(out, "\"type\":\"Position\"") == NULL, "missing type: %s", out);
    FAIL_IF(strstr(out, "\"planogram_key\":7") == NULL, "missing planogram_key: %s", out);
    FAIL_IF(strstr(out, "\"x_cap\":[2,0,0,0]") == NULL, "missing x_cap: %s", out);
    FAIL_IF(strstr(out, "\"merch_xyz\":[0,1,2") == NULL, "missing merch_xyz: %s", out);
    FAIL_IF(strstr(out, "\"target_val_x\":0") == NULL, "missing target_val_x: %s", out);
    FAIL_IF(strstr(out, "\"x_cap_includes_units\":0") == NULL, "missing x_cap_includes_units: %s", out);
    return 0;
}

static int test_performance_json(void)
{
    psa_record_t rec;
    char out[4096];
    size_t n, need;
    int rc;

    memset(&rec, 0, sizeof(rec));
    rec.type = PSA_REC_PERFORMANCE;
    populate_performance(&rec.rec.performance);

    rc = psa_record_to_json(&rec, out, sizeof(out), &n, &need);
    FAIL_IF(rc != PSA_OK, "performance json failed: %d", rc);
    FAIL_IF(strstr(out, "\"type\":\"Performance\"") == NULL, "missing type: %s", out);
    FAIL_IF(strstr(out, "\"unit_movement\":100") == NULL, "missing unit_movement: %s", out);
    FAIL_IF(strstr(out, "\"combined_perf\":0.75") == NULL, "missing combined_perf: %s", out);
    FAIL_IF(strstr(out, "\"assortment_rank\":1") == NULL, "missing assortment_rank: %s", out);
    FAIL_IF(strstr(out, "\"target_dist_percent\":0.8") == NULL, "missing target_dist_percent: %s", out);
    FAIL_IF(strstr(out, "\"max_stage_reduction\":0.1") == NULL, "missing max_stage_reduction: %s", out);
    return 0;
}

static int test_segment_json(void)
{
    psa_record_t rec;
    char out[2048];
    size_t n, need;
    int rc;

    memset(&rec, 0, sizeof(rec));
    rec.type = PSA_REC_SEGMENT;
    populate_segment(&rec.rec.segment);

    rc = psa_record_to_json(&rec, out, sizeof(out), &n, &need);
    FAIL_IF(rc != PSA_OK, "segment json failed: %d", rc);
    FAIL_IF(strstr(out, "\"type\":\"Segment\"") == NULL, "missing type: %s", out);
    FAIL_IF(strstr(out, "\"planogram_key\":7") == NULL, "missing planogram_key: %s", out);
    FAIL_IF(strstr(out, "\"door_flag\":0") == NULL, "missing door_flag: %s", out);
    FAIL_IF(strstr(out, "\"frame_width\":1") == NULL, "missing frame_width: %s", out);
    return 0;
}

static int test_drawing_json(void)
{
    psa_record_t rec;
    char out[2048];
    size_t n, need;
    int rc;

    memset(&rec, 0, sizeof(rec));
    rec.type = PSA_REC_DRAWING;
    populate_drawing(&rec.rec.drawing);

    rc = psa_record_to_json(&rec, out, sizeof(out), &n, &need);
    FAIL_IF(rc != PSA_OK, "drawing json failed: %d", rc);
    FAIL_IF(strstr(out, "\"type\":\"Drawing\"") == NULL, "missing type: %s", out);
    FAIL_IF(strstr(out, "\"drawing_type\":1") == NULL, "missing drawing_type: %s", out);
    FAIL_IF(strstr(out, "\"text\":\"Hello\\nWorld\"") == NULL, "missing text or bad escape: %s", out);
    FAIL_IF(strstr(out, "\"font_metrics\":[1000000000") == NULL, "missing font_metrics: %s", out);
    FAIL_IF(strstr(out, "\"font_style\":[1,0") == NULL, "missing font_style: %s", out);
    return 0;
}

static int test_divider_json(void)
{
    psa_record_t rec;
    char out[1024];
    size_t n, need;
    int rc;

    memset(&rec, 0, sizeof(rec));
    rec.type = PSA_REC_DIVIDER;
    populate_divider(&rec.rec.divider);

    rc = psa_record_to_json(&rec, out, sizeof(out), &n, &need);
    FAIL_IF(rc != PSA_OK, "divider json failed: %d", rc);
    FAIL_IF(strstr(out, "\"type\":\"Divider\"") == NULL, "missing type: %s", out);
    FAIL_IF(strstr(out, "\"id\":\"DIV-001\"") == NULL, "missing id: %s", out);
    FAIL_IF(strstr(out, "\"num_1\":1") == NULL, "missing num_1: %s", out);
    return 0;
}

/* -------------------------------------------------------------------------- */
/* Edge cases: null strings, escaping, non-finite doubles                    */
/* -------------------------------------------------------------------------- */

static int test_null_strings(void)
{
    psa_record_t rec;
    char out[1024];
    size_t n, need;
    int rc;

    memset(&rec, 0, sizeof(rec));
    rec.type = PSA_REC_DIVIDER;
    rec.rec.divider.id = NULL;
    rec.rec.divider.desc_text_1 = NULL;

    rc = psa_record_to_json(&rec, out, sizeof(out), &n, &need);
    FAIL_IF(rc != PSA_OK, "null string json failed: %d", rc);
    FAIL_IF(strstr(out, "\"id\":\"\"") == NULL, "null id should be empty string: %s", out);
    FAIL_IF(strstr(out, "\"desc_text_1\":\"\"") == NULL, "null desc_text_1 should be empty string: %s", out);
    return 0;
}

static int test_json_escaping(void)
{
    psa_record_t rec;
    char out[2048];
    size_t n, need;
    int rc;

    memset(&rec, 0, sizeof(rec));
    rec.type = PSA_REC_DIVIDER;
    rec.rec.divider.id = "tab\there\nnewline\rcarriage\bbackspace\fformfeed\\slash\"quote";
    rec.rec.divider.desc_text_1 = "\x01\x02\x03ctrl"; /* control chars */

    rc = psa_record_to_json(&rec, out, sizeof(out), &n, &need);
    FAIL_IF(rc != PSA_OK, "escaping json failed: %d", rc);
    FAIL_IF(strstr(out, "\\t") == NULL, "missing tab escape: %s", out);
    FAIL_IF(strstr(out, "\\n") == NULL, "missing newline escape: %s", out);
    FAIL_IF(strstr(out, "\\r") == NULL, "missing carriage return escape: %s", out);
    FAIL_IF(strstr(out, "\\b") == NULL, "missing backspace escape: %s", out);
    FAIL_IF(strstr(out, "\\f") == NULL, "missing formfeed escape: %s", out);
    FAIL_IF(strstr(out, "\\\\") == NULL, "missing slash escape: %s", out);
    FAIL_IF(strstr(out, "\\\"") == NULL, "missing quote escape: %s", out);
    FAIL_IF(strstr(out, "\\u0001") == NULL, "missing ctrl escape: %s", out);
    return 0;
}

static int test_nonfinite_double(void)
{
    psa_record_t rec;
    char out[1024];
    size_t n, need;
    int rc;

    memset(&rec, 0, sizeof(rec));
    rec.type = PSA_REC_SEGMENT;
    populate_segment(&rec.rec.segment);
    rec.rec.segment.frame_width = NAN;

    rc = psa_record_to_json(&rec, out, sizeof(out), &n, &need);
    FAIL_IF(rc != PSA_ERR_NONFINITE, "expected NONFINITE for NaN, got %d", rc);

    rec.rec.segment.frame_width = INFINITY;
    rc = psa_record_to_json(&rec, out, sizeof(out), &n, &need);
    FAIL_IF(rc != PSA_ERR_NONFINITE, "expected NONFINITE for Inf, got %d", rc);

    rec.rec.segment.frame_width = -INFINITY;
    rc = psa_record_to_json(&rec, out, sizeof(out), &n, &need);
    FAIL_IF(rc != PSA_ERR_NONFINITE, "expected NONFINITE for -Inf, got %d", rc);

    return 0;
}

/* -------------------------------------------------------------------------- */
/* Document API tests with synthetic files                                    */
/* -------------------------------------------------------------------------- */

static int test_document_api_file(void)
{
    const char *path = "synthetic/out/SYN_0001_minimal_defaults_2026_05_18_10_00_01.psa";
    char errbuf[1024] = {0};
    size_t cap = 256 * 1024;
    char *doc = malloc(cap);
    size_t n = 0;
    size_t need = 0;
    int rc;

    FAIL_IF(!doc, "malloc failed");

    rc = psa_parse_file_to_json_document(path, doc, cap, &n, &need, errbuf, sizeof(errbuf));
    FAIL_IF(rc != PSA_OK, "document API file failed: rc=%d err=%s", rc, errbuf);
    FAIL_IF(n == 0, "document API wrote empty output");

    FAIL_IF(strstr(doc, "\"header\"") == NULL, "missing header: %s", doc);
    FAIL_IF(strstr(doc, "\"version\"") == NULL, "missing version: %s", doc);
    FAIL_IF(strstr(doc, "\"projects\":[{") == NULL, "missing projects: %s", doc);
    FAIL_IF(strstr(doc, "\"planograms\":[{") == NULL, "missing planograms: %s", doc);
    FAIL_IF(strstr(doc, "\"products\":[{") == NULL, "missing products: %s", doc);
    FAIL_IF(strstr(doc, "\"performances\":[{") == NULL, "missing performances: %s", doc);
    FAIL_IF(strstr(doc, "\"segments\":[{") == NULL, "missing segments: %s", doc);
    FAIL_IF(strstr(doc, "\"fixtures\":[{") == NULL, "missing fixtures: %s", doc);
    FAIL_IF(strstr(doc, "\"positions\":[{") == NULL, "missing positions: %s", doc);
    FAIL_IF(strstr(doc, "\"drawings\":[]") == NULL, "missing empty drawings: %s", doc);
    FAIL_IF(strstr(doc, "\"dividers\":[]") == NULL, "missing empty dividers: %s", doc);

    free(doc);
    return 0;
}

static int test_document_api_buffer(void)
{
    const char *path = "synthetic/out/SYN_0002_small_realistic_2026_05_18_10_00_02.psa";
    FILE *fp = fopen(path, "rb");
    char errbuf[1024] = {0};
    size_t cap = 256 * 1024;
    char *doc = malloc(cap);
    char *buf = NULL;
    size_t buf_len = 0;
    size_t n = 0;
    size_t need = 0;
    int rc;

    FAIL_IF(!fp, "failed to open synthetic file");
    FAIL_IF(!doc, "malloc failed");

    fseek(fp, 0, SEEK_END);
    buf_len = (size_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buf = malloc(buf_len + 1);
    FAIL_IF(!buf, "malloc buf failed");
    FAIL_IF(fread(buf, 1, buf_len, fp) != buf_len, "fread failed");
    buf[buf_len] = '\0';
    fclose(fp);

    rc = psa_parse_buffer_to_json_document(buf, buf_len, doc, cap, &n, &need, errbuf, sizeof(errbuf));
    FAIL_IF(rc != PSA_OK, "buffer document API failed: rc=%d err=%s", rc, errbuf);
    FAIL_IF(n == 0, "buffer document API wrote empty output");

    FAIL_IF(strstr(doc, "\"header\"") == NULL, "missing header in buffer doc: %s", doc);
    FAIL_IF(strstr(doc, "\"drawings\":[{") == NULL, "missing drawings in buffer doc: %s", doc);
    FAIL_IF(strstr(doc, "\"dividers\":[]") == NULL, "missing empty dividers in buffer doc: %s", doc);

    free(buf);
    free(doc);
    return 0;
}

static int test_document_api_nospace(void)
{
    const char *path = "synthetic/out/SYN_0001_minimal_defaults_2026_05_18_10_00_01.psa";
    char errbuf[1024] = {0};
    char tiny[16];
    size_t n = 0;
    size_t need = 0;
    int rc;

    rc = psa_parse_file_to_json_document(path, tiny, sizeof(tiny), &n, &need, errbuf, sizeof(errbuf));
    FAIL_IF(rc != PSA_ERR_NOSPACE, "expected NOSPACE, got %d", rc);
    FAIL_IF(need <= sizeof(tiny), "expected need > %zu, got %zu", sizeof(tiny), need);
    return 0;
}

/* -------------------------------------------------------------------------- */
/* Error paths                                                                */
/* -------------------------------------------------------------------------- */

static int test_invalid_args(void)
{
    char out[64];
    size_t n, need;
    int rc;

    rc = psa_file_meta_to_json("h", "v", NULL, 1, &n, &need);
    FAIL_IF(rc != PSA_ERR_INVALID_ARG, "expected INVALID_ARG for NULL out with size>0, got %d", rc);

    rc = psa_parse_file_to_json_document(NULL, out, sizeof(out), &n, &need, NULL, 0);
    FAIL_IF(rc != PSA_ERR_INVALID_ARG, "expected INVALID_ARG for NULL path, got %d", rc);

    rc = psa_parse_buffer_to_json_document(NULL, 1, out, sizeof(out), &n, &need, NULL, 0);
    FAIL_IF(rc != PSA_ERR_INVALID_ARG, "expected INVALID_ARG for NULL buf with len>0, got %d", rc);

    return 0;
}

/* -------------------------------------------------------------------------- */
/* main                                                                       */
/* -------------------------------------------------------------------------- */

int main(void)
{
    int rc = 0;
    rc |= test_file_meta_json();
    rc |= test_record_json();
    rc |= test_project_json();
    rc |= test_planogram_json();
    rc |= test_fixture_json();
    rc |= test_product_json();
    rc |= test_position_json();
    rc |= test_performance_json();
    rc |= test_segment_json();
    rc |= test_drawing_json();
    rc |= test_divider_json();
    rc |= test_null_strings();
    rc |= test_json_escaping();
    rc |= test_nonfinite_double();
    rc |= test_document_api_file();
    rc |= test_document_api_buffer();
    rc |= test_document_api_nospace();
    rc |= test_invalid_args();

    if (rc == 0)
        printf("json: all tests passed\n");
    return rc;
}
