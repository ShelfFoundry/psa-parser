#include "psa.h"
#include <string.h>
#include <stdlib.h>

/* ------------------------------------------------------------------------- */
/* Helper macros                                                             */
/* ------------------------------------------------------------------------- */

#define STR(f, idx, def)  do { \
    if ((size_t)(idx) < nfields) { \
        const char *_s = fields[(idx)]; \
        (f) = (_s && _s[0]) ? _s : (def); \
    } else { (f) = (def); } \
} while(0)

#define INT(f, idx, def)  do { \
    (f) = ((size_t)(idx) < nfields) ? psa_parse_int(fields[(idx)], (def)) : (def); \
} while(0)

#define DBL(f, idx, def)  do { \
    (f) = ((size_t)(idx) < nfields) ? psa_parse_double(fields[(idx)], (def)) : (def); \
} while(0)

#define I64(f, idx, def)  do { \
    (f) = ((size_t)(idx) < nfields) ? psa_parse_int64(fields[(idx)], (def)) : (def); \
} while(0)

#define REQ_MIN(n) do { if (nfields < (size_t)(n)) return -1; } while(0)

/* ------------------------------------------------------------------------- */
/* Project                                                                   */
/* ------------------------------------------------------------------------- */

int psa_parse_project(char **fields, size_t nfields, psa_project_t *p)
{
    REQ_MIN(213);   /* columns 0..212 */

    memset(p, 0, sizeof(*p));
    p->family_key = -1;

    STR(p->display_name,   1,  "Space Planning Project");
    STR(p->key_text,       2,  "");
    INT(p->primary_key,    3,  0);
    STR(p->layout_file,    4,  "");
    INT(p->movement_period,5,  7);
    DBL(p->case_multiple,  6,  1.5);
    DBL(p->days_supply,    7,  1.5);
    INT(p->demand_cycle,   8,  1);
    DBL(p->peak_safety,    9,  1.0);
    DBL(p->backroom_stock, 10, 0.0);
    STR(p->peg_profile,    11, "");
    INT(p->measurement_mode,12,0);
    INT(p->num_stores,     13, 0);

    for (int i = 0; i < 9; i++)  INT(p->merch_x[i], 14 + i, 0);
    for (int i = 0; i < 9; i++)  INT(p->merch_y[i], 23 + i, 0);
    for (int i = 0; i < 9; i++)  INT(p->merch_z[i], 32 + i, 0);

    for (int i = 0; i < 28; i++) DBL(p->demand[i],  41 + i, 0.0);
    for (int i = 0; i < 6;  i++) INT(p->inv_model_opts[i], 69 + i, 0);
    for (int i = 0; i < 50; i++) DBL(p->num_ext[i], 75 + i, 0.0);
    for (int i = 0; i < 50; i++) STR(p->text_ext[i], 125 + i, "");
    for (int i = 0; i < 10; i++) INT(p->flag_ext[i], 175 + i, 0);

    STR(p->notes,          185, "");
    INT(p->changed,         186, 0);
    for (int i = 0; i < 10; i++) I64(p->ext_db_keys[i], 187 + i, -1);
    for (int i = 0; i < 4;  i++) INT(p->perf_override[i], 197 + i, 2);
    STR(p->status,          201, "");
    for (int i = 0; i < 8;  i++) INT(p->date_slots[i], 202 + i, 0);
    STR(p->created_by,      210, "");
    STR(p->modified_by,      211, "");
    INT(p->inv_mode,         212, 0);

    /* Optional tail */
    if (nfields >= 214) STR(p->delivery_schedule, 213, "");
    if (nfields >= 215) STR(p->custom_payload,    214, "");
    if (nfields >= 216) INT(p->family_key,       215, -1);

    return 0;
}

/* ------------------------------------------------------------------------- */
/* Planogram                                                                 */
/* ------------------------------------------------------------------------- */

int psa_parse_planogram(char **fields, size_t nfields, psa_planogram_t *p)
{
    REQ_MIN(229);   /* columns 0..228 */

    memset(p, 0, sizeof(*p));
    p->notch_peg[0] = -1.0;
    p->alloc_max_target = -1.0;
    p->split_ctrl = -1;
    p->segment_ctrl = -1;
    p->status_ctrl = -1;
    p->score_ctrl = -1;
    p->warning_count = -1;
    p->error_count = -1;
    p->stage_limit = -1;
    p->type_ref = -1;
    p->model_ref = -1;
    p->family_ref = -1;
    p->version_ref = -1;
    p->parent_ref = -1;
    p->processing_ts = -1;
    p->final_status = -1;
    p->optb_250 = -1;
    p->optb_251 = -1;
    p->optb_252 = -1;
    p->optb_253 = -1;
    p->action_text = "";
    p->server_text = "";
    p->optb_254 = "";

    STR(p->name,           1,  "");
    STR(p->key_text,       2,  "");
    DBL(p->width,          3,  0.0);
    DBL(p->height,         4,  0.0);
    DBL(p->depth,          5,  0.0);
    INT(p->display_color,  6,  0);
    DBL(p->back_depth,     7,  0.0);
    INT(p->draw_back,      8,  1);
    DBL(p->base_width,     9,  0.0);
    DBL(p->base_height,    10, 0.0);
    DBL(p->base_depth,     11, 0.0);
    INT(p->draw_base,      12, 1);
    INT(p->base_color,     13, 0);

    DBL(p->notch_peg[0], 14, -1.0);
    for (int i = 1; i < 8; i++) DBL(p->notch_peg[i], 14 + i, 0.0);

    INT(p->traffic_flow,   22, 0);
    INT(p->auto_created,   23, 0);
    STR(p->shape_ref,      24, "");
    STR(p->bitmap_ref,     25, "");

    for (int i = 0; i < 9; i++) INT(p->merch_x[i], 26 + i, 0);
    for (int i = 0; i < 9; i++) INT(p->merch_y[i], 35 + i, 0);
    for (int i = 0; i < 9; i++) INT(p->merch_z[i], 44 + i, 0);

    DBL(p->combined_perf,  53, 0.0);
    INT(p->store_count,    54, 0);
    DBL(p->notch_width,    55, 0.0);
    for (int i = 0; i < 50; i++) STR(p->text_ext[i], 56 + i, "");
    for (int i = 0; i < 50; i++) DBL(p->num_ext[i], 106 + i, 0.0);
    for (int i = 0; i < 10; i++) INT(p->flag_ext[i], 156 + i, 0);

    INT(p->fill_pattern,   166, 0);
    STR(p->printable_segments, 167, "");
    STR(p->source_file,    168, "");
    INT(p->changed,        169, 0);
    STR(p->layout_file,    170, "");
    STR(p->notes,          171, "");
    for (int i = 0; i < 10; i++) I64(p->ext_db_keys[i], 172 + i, -1);
    INT(p->source_type,    182, 2);
    for (int i = 0; i < 3;  i++) STR(p->status[i], 183 + i, "");
    for (int i = 0; i < 8;  i++) INT(p->date_slots[i], 186 + i, 0);
    STR(p->created_by,     194, "");
    STR(p->modified_by,     195, "");
    STR(p->floor_bitmap_ref,196, "");
    DBL(p->door_transparency,197,0.5);
    DBL(p->floor_tile_width,198,12.0);
    DBL(p->floor_tile_depth,199,12.0);
    for (int i = 0; i < 6;  i++) INT(p->inv_model_opts[i], 200 + i, 0);
    DBL(p->case_multiple,  206, 1.5);
    DBL(p->days_supply,     207, 1.5);
    INT(p->demand_cycle,   208, 1);
    DBL(p->peak_safety,     209, 1.0);
    DBL(p->backroom_stock,  210, 0.0);
    for (int i = 0; i < 7;  i++) DBL(p->demand[i], 211 + i, 0.0);
    STR(p->delivery_schedule,218,"");
    STR(p->business_id,    219, "");
    STR(p->department,       220, "");
    STR(p->part_id,        221, "");
    STR(p->gln_code,       222, "");
    STR(p->custom_payload,  223, "");
    STR(p->guid,           224, "");
    STR(p->db_guid,         225, "");
    STR(p->abbrev_name,     226, "");
    STR(p->category,        227, "");
    STR(p->subcategory,     228, "");

    /* Optional section A: strict 229..232 */
    if (nfields > 229) {
        REQ_MIN(233);   /* must have all 4 fields */
        INT(p->opt_source_code,     229, 0);
        STR(p->allocation_group,   230, "");
        INT(p->allocation_sequence,231, 0);
        DBL(p->alloc_min_target,    232, 0.0);
    }

    /* Optional section B: strict 233..254 */
    if (nfields > 233) {
        REQ_MIN(255);   /* must have all 22 fields */
        DBL(p->alloc_max_target,    233, -1.0);
        INT(p->split_ctrl,          234, -1);
        INT(p->segment_ctrl,        235, -1);
        INT(p->status_ctrl,         236, -1);
        INT(p->score_ctrl,          237, -1);
        INT(p->warning_count,       238, -1);
        INT(p->error_count,         239, -1);
        STR(p->action_text,         240, "");
        INT(p->stage_limit,         241, -1);
        INT(p->type_ref,            242, -1);
        INT(p->model_ref,           243, -1);
        INT(p->family_ref,          244, -1);
        INT(p->version_ref,         245, -1);
        INT(p->parent_ref,          246, -1);
        INT(p->processing_ts,       247, -1);
        STR(p->server_text,         248, "");
        INT(p->final_status,        249, -1);
        INT(p->optb_250,            250, -1);
        INT(p->optb_251,            251, -1);
        INT(p->optb_252,            252, -1);
        INT(p->optb_253,            253, -1);
        STR(p->optb_254,            254, "");
    }

    return 0;
}

/* ------------------------------------------------------------------------- */
/* Fixture                                                                   */
/* ------------------------------------------------------------------------- */

int psa_parse_fixture(char **fields, size_t nfields, psa_fixture_t *p, int planogram_key)
{
    REQ_MIN(158);   /* columns 1..157 + indicator */

    memset(p, 0, sizeof(*p));
    p->planogram_key = planogram_key;

    INT(p->type_code,      1,  0);
    STR(p->name,           2,  "");
    STR(p->key_text,       3,  "");
    DBL(p->x,              4,  0.0);
    DBL(p->width,          5,  0.0);
    DBL(p->y,              6,  0.0);
    DBL(p->height,         7,  0.0);
    DBL(p->z,              8,  0.0);
    DBL(p->depth,          9,  0.0);
    DBL(p->slope,          10, 0.0);
    DBL(p->angle,          11, 0.0);
    DBL(p->roll,           12, 0.0);
    INT(p->color,          13, 0);
    STR(p->assembly,       14, "");

    for (int i = 0; i < 9; i++) DBL(p->fixture_params[i], 15 + i, 0.0);

    INT(p->collision_fixtures, 24, 1);
    INT(p->collision_positions,25, 1);
    INT(p->can_obstruct,   26, 0);

    for (int i = 0; i < 6; i++) DBL(p->overhang[i], 27 + i, 0.0);

    INT(p->default_merch_style,33, -1);
    for (int i = 0; i < 3; i++) DBL(p->divider_dim[i], 34 + i, 0.0);
    INT(p->combinable,     37, 0);
    for (int i = 0; i < 7; i++) DBL(p->grille_notch_peg[i], 38 + i, 0.0);

    STR(p->primary_label,  45, "");
    STR(p->secondary_label,46, "");
    STR(p->shape_ref,      47, "");
    STR(p->bitmap_ref,     48, "");

    for (int i = 0; i < 9; i++) INT(p->merch_x[i], 49 + i, 0);
    for (int i = 0; i < 9; i++) INT(p->merch_y[i], 58 + i, 0);
    for (int i = 0; i < 9; i++) INT(p->merch_z[i], 67 + i, 0);

    for (int i = 0; i < 30; i++) STR(p->text_ext[i], 76 + i, "");
    for (int i = 0; i < 30; i++) DBL(p->num_ext[i], 106 + i, 0.0);
    for (int i = 0; i < 10; i++) INT(p->flag_ext[i], 136 + i, 0);

    INT(p->location_id,    146, -1);
    INT(p->fill_pattern,   147, 0);
    STR(p->model_file,     148, "");
    DBL(p->weight_capacity,149, 0.0);
    INT(p->changed,        150, 0);
    for (int i = 0; i < 3; i++) INT(p->divider_placement[i], 151 + i, 0);
    DBL(p->transparency,   154, 0.0);
    INT(p->hide_when_printing,155,0);
    STR(p->product_assoc,  156, "");
    STR(p->part_id,        157, "");

    /* Optional tail 158..159 */
    if (nfields > 158) {
        REQ_MIN(160);   /* strict: need both 158 and 159 */
        INT(p->hide_view_dims, 158, 0);
        STR(p->gln_code,       159, "");
    }

    return 0;
}

/* ------------------------------------------------------------------------- */
/* Product                                                                   */
/* ------------------------------------------------------------------------- */

int psa_parse_product(char **fields, size_t nfields, psa_product_t *p)
{
    REQ_MIN(274);   /* columns 1..273 */

    memset(p, 0, sizeof(*p));

    STR(p->upc,            1,  "");
    STR(p->business_id,    2,  "");
    STR(p->name,           3,  "");
    STR(p->key_text,       4,  "");
    DBL(p->width,          5,  0.0);
    DBL(p->height,         6,  0.0);
    DBL(p->depth,          7,  0.0);
    INT(p->color,          8,  0);
    STR(p->abbrev_name,    9,  "");
    DBL(p->size,            10, 0.0);
    STR(p->uom,            11, "");
    STR(p->manufacturer,   12, "");
    STR(p->category,       13, "");
    STR(p->supplier,       14, "");
    INT(p->inner_pack_qty, 15, 0);
    DBL(p->nesting_x,      16, 0.0);
    DBL(p->nesting_y,      17, 0.0);
    DBL(p->nesting_z,      18, 0.0);
    INT(p->peg_hole_count, 19, 1);
    for (int i = 0; i < 9; i++) DBL(p->peg_hole_geom[i], 20 + i, 0.0);
    INT(p->packaging_style,29, 0);
    STR(p->peg_profile,    30, "");
    DBL(p->finger_space_y, 31, 0.0);
    DBL(p->jumble_factor,  32, 0.0);
    DBL(p->price,          33, 0.0);
    DBL(p->case_cost,      34, 0.0);
    INT(p->tax_code,       35, 1);
    DBL(p->unit_movement,  36, 0.0);
    DBL(p->share,          37, 0.0);
    DBL(p->case_multiple,  38, 0.0);
    DBL(p->days_supply,     39, 0.0);
    DBL(p->combined_perf,   40, 0.0);
    INT(p->peg_span,       41, 0);
    INT(p->min_units,      42, 0);
    INT(p->max_units,      43, 0);
    STR(p->shape_ref,      44, "");
    STR(p->bitmap_ref,     45, "");

    for (int i = 0; i < 8; i++) DBL(p->tray[i],      46 + i, 0.0);
    for (int i = 0; i < 8; i++) DBL(p->case_pack[i], 54 + i, 0.0);
    for (int i = 0; i < 8; i++) DBL(p->display[i],   62 + i, 0.0);
    for (int i = 0; i < 8; i++) DBL(p->alternate[i], 70 + i, 0.0);
    for (int i = 0; i < 8; i++) DBL(p->loose[i],     78 + i, 0.0);

    for (int i = 0; i < 27; i++) INT(p->merch_xyz[i], 86 + i, 0);

    INT(p->num_positions,  113, 1);
    for (int i = 0; i < 50; i++) STR(p->text_ext[i], 114 + i, "");
    for (int i = 0; i < 50; i++) DBL(p->num_ext[i], 164 + i, 0.0);
    for (int i = 0; i < 10; i++) INT(p->flag_ext[i], 214 + i, 0);

    DBL(p->squeeze_min_x,  224, 1.0);
    DBL(p->squeeze_max_x,  225, 1.0);
    DBL(p->squeeze_min_y,  226, 1.0);
    DBL(p->squeeze_max_y,  227, 1.0);
    DBL(p->squeeze_min_z,  228, 1.0);
    DBL(p->squeeze_max_z,  229, 1.0);
    INT(p->fill_pattern,   230, 0);
    STR(p->model_file,     231, "");
    STR(p->brand,          232, "");
    STR(p->subcategory,    233, "");
    DBL(p->weight,         234, 0.0);
    STR(p->planogram_alias,235, "");
    INT(p->changed,        236, 0);
    DBL(p->front_overhang, 237, 0.0);
    DBL(p->finger_space_x, 238, 0.0);
    for (int i = 0; i < 10; i++) I64(p->ext_db_keys[i], 239 + i, 0);
    STR(p->status,         249, "");
    for (int i = 0; i < 8;  i++) INT(p->date_slots[i], 250 + i, 0);
    STR(p->created_by,     258, "");
    STR(p->modified_by,     259, "");
    DBL(p->transparency,    260, 0.0);
    DBL(p->peak_safety,     261, 0.0);
    DBL(p->backroom_stock,  262, 0.0);
    STR(p->delivery_schedule,263,"");
    STR(p->part_id,         264, "");
    INT(p->authority_level, 265, 0);
    INT(p->bitmap_unit_override,266,0);
    INT(p->model_lookup_mode,267, 0);
    INT(p->default_merch_style,268,0);
    INT(p->auto_model,       269, 0);
    STR(p->custom_payload,   270, "");
    STR(p->db_guid,          271, "");
    INT(p->source_code,      272, 0);
    I64(p->technical_key,    273, 0);

    return 0;
}

/* ------------------------------------------------------------------------- */
/* Position                                                                  */
/* ------------------------------------------------------------------------- */

int psa_parse_position(char **fields, size_t nfields, psa_position_t *p, int planogram_key)
{
    REQ_MIN(167);   /* columns 1..166 */

    memset(p, 0, sizeof(*p));
    p->planogram_key = planogram_key;

    STR(p->upc,            1,  "");
    STR(p->business_id,    2,  "");
    STR(p->key_text,       3,  "");
    DBL(p->x,              4,  0.0);
    DBL(p->width,          5,  0.0);
    DBL(p->y,              6,  0.0);
    DBL(p->height,         7,  0.0);
    DBL(p->z,              8,  0.0);
    DBL(p->depth,          9,  0.0);
    DBL(p->slope,          10, 0.0);
    DBL(p->angle,          11, 0.0);
    DBL(p->roll,           12, 0.0);
    INT(p->merch_style,    13, 0);
    INT(p->h_facing,       14, 1);
    INT(p->v_facing,       15, 1);
    INT(p->d_facing,       16, 1);

    for (int i = 0; i < 4; i++) INT(p->x_cap[i], 17 + i, (i==3)?2:0);
    for (int i = 0; i < 4; i++) INT(p->y_cap[i], 21 + i, (i==3)?4:0);
    for (int i = 0; i < 4; i++) INT(p->z_cap[i], 25 + i, (i==0)?1:((i==3)?2:0));

    INT(p->orientation,    29, 0);
    DBL(p->jumble_x,       30, 0.0);
    DBL(p->jumble_y,       31, 0.0);
    DBL(p->jumble_z,       32, 0.0);
    DBL(p->merch_dim_x,    33, 0.0);
    DBL(p->merch_dim_y,    34, 0.0);
    DBL(p->merch_dim_z,    35, 0.0);
    DBL(p->full_dim_x,     36, 0.0);
    DBL(p->full_dim_y,     37, 0.0);
    DBL(p->full_dim_z,     38, 0.0);
    INT(p->subunit_x,      39, 0);
    INT(p->subunit_y,      40, 0);
    INT(p->subunit_z,      41, 0);
    STR(p->peg_profile,    42, "");
    INT(p->manual_units,   43, 1);
    INT(p->rank_x,         44, 1);
    INT(p->rank_y,         45, 1);
    INT(p->rank_z,         46, 1);
    INT(p->peg_span,       47, 0);
    INT(p->always_float,   48, 0);
    STR(p->primary_label,  49, "");
    STR(p->secondary_label,50, "");

    for (int i = 0; i < 27; i++) INT(p->merch_xyz[i], 51 + i, 0);

    for (int i = 0; i < 30; i++) STR(p->text_ext[i], 78 + i, "");
    for (int i = 0; i < 30; i++) DBL(p->num_ext[i], 108 + i, 0.0);
    for (int i = 0; i < 10; i++) INT(p->flag_ext[i], 138 + i, 0);

    INT(p->target_space_x, 148, 0);
    INT(p->target_space_y, 149, 0);
    INT(p->target_space_z, 150, 0);
    DBL(p->target_val_x,   151, 0.0);
    DBL(p->target_val_y,   152, 0.0);
    DBL(p->target_val_z,   153, 0.0);
    INT(p->location_id,    154, -1);
    INT(p->changed,        155, 0);
    INT(p->replenishment_min,156,0);
    INT(p->replenishment_max,157,0);
    STR(p->shape_ref,      158, "");
    STR(p->bitmap_ref,     159, "");
    INT(p->hide_when_printing,160,0);
    STR(p->part_id,        161, "");
    INT(p->bitmap_unit_override,162,0);
    INT(p->auto_model,     163, -1);
    STR(p->custom_payload, 164, "");
    INT(p->x_cap_includes_units,165,0);
    INT(p->y_cap_includes_units,166,0);

    return 0;
}

/* ------------------------------------------------------------------------- */
/* Performance                                                               */
/* ------------------------------------------------------------------------- */

int psa_parse_performance(char **fields, size_t nfields, psa_performance_t *p)
{
    REQ_MIN(150);   /* columns 1..149 */

    memset(p, 0, sizeof(*p));

    STR(p->upc,            1,  "");
    STR(p->business_id,    2,  "");
    STR(p->key_text,       3,  "");
    DBL(p->price,          4,  0.0);
    DBL(p->case_cost,       5,  0.0);
    INT(p->tax_code,       6,  1);
    DBL(p->unit_movement,   7,  0.0);
    DBL(p->share,           8,  0.0);
    DBL(p->combined_perf,   9,  0.0);

    for (int i = 0; i < 10; i++) STR(p->text_ext[i], 10 + i, "");
    for (int i = 0; i < 10; i++) DBL(p->num_ext[i], 20 + i, 0.0);
    for (int i = 0; i < 10; i++) INT(p->flag_ext[i], 30 + i, 0);

    INT(p->changed,        40, 0);
    for (int i = 0; i < 20; i++) STR(p->text_ext[10 + i], 41 + i, "");
    for (int i = 0; i < 20; i++) DBL(p->num_ext[10 + i], 61 + i, 0.0);

    DBL(p->case_multiple,  81, 0.0);
    DBL(p->days_supply,     82, 0.0);
    DBL(p->peak_safety,     83, -0.01);
    DBL(p->backroom_stock,  84, -0.01);
    INT(p->min_units,       85, -1);
    INT(p->max_units,       86, -1);
    STR(p->delivery_schedule,87,"");
    INT(p->replenishment_min,88,0);
    INT(p->replenishment_max,89,0);
    INT(p->assortment_rank, 90, 0);
    INT(p->recommended_facings,91,0);
    STR(p->assortment_strategy,92,"");
    STR(p->assortment_tactic,93,"");
    STR(p->assortment_reason,94,"");
    STR(p->assortment_action,95,"");
    STR(p->part_id,         96, "");
    STR(p->cluster_name,      97, "");
    INT(p->target_store_count,98,0);
    DBL(p->target_dist_percent,99,0.0);
    STR(p->assortment_note, 100, "");

    for (int i = 0; i < 20; i++) STR(p->text_ext[30 + i], 101 + i, "");
    for (int i = 0; i < 20; i++) DBL(p->num_ext[30 + i], 121 + i, 0.0);

    STR(p->custom_payload,   141, "");
    INT(p->recommended_orientation,142,-1);
    INT(p->recommended_merch_style,143,-1);
    INT(p->ignore_recommendations,144,0);
    INT(p->priority_code,    145, 0);
    STR(p->priority_desc,    146, "");
    INT(p->force_list,       147, 0);
    STR(p->planogram_reason, 148, "");
    DBL(p->max_stage_reduction,149,0.0);

    return 0;
}

/* ------------------------------------------------------------------------- */
/* Segment                                                                   */
/* ------------------------------------------------------------------------- */

int psa_parse_segment(char **fields, size_t nfields, psa_segment_t *p, int planogram_key)
{
    REQ_MIN(12);    /* columns 1..11 (base) */

    memset(p, 0, sizeof(*p));
    p->planogram_key = planogram_key;

    STR(p->name,           1,  "");
    STR(p->key_text,       2,  "");
    DBL(p->x,              3,  0.0);
    DBL(p->width,          4,  0.0);
    DBL(p->y,              5,  0.0);
    DBL(p->height,         6,  0.0);
    DBL(p->z,              7,  0.0);
    DBL(p->depth,          8,  0.0);
    DBL(p->angle,          9,  0.0);
    DBL(p->x_offset,       10, 0.0);
    DBL(p->y_offset,       11, 0.0);

    /* Extended section: strict 12..51 */
    if (nfields > 12) {
        REQ_MIN(52);
        INT(p->door_flag,        12, 0);
        INT(p->door_direction,    13, 0);
        for (int i = 0; i < 10; i++) STR(p->text_ext[i], 14 + i, "");
        for (int i = 0; i < 10; i++) DBL(p->num_ext[i], 24 + i, 0.0);
        for (int i = 0; i < 10; i++) INT(p->flag_ext[i], 34 + i, 0);
        DBL(p->frame_width,       44, 0.0);
        DBL(p->frame_height,      45, 0.0);
        INT(p->changed,           46, 0);
        INT(p->frame_color,       47, -1);
        INT(p->frame_fill_pattern,48, 0);
        STR(p->part_id,          49, "");
        STR(p->gln_code,         50, "");
        STR(p->custom_payload,    51, "");
    }

    return 0;
}

/* ------------------------------------------------------------------------- */
/* Drawing                                                                   */
/* ------------------------------------------------------------------------- */

int psa_parse_drawing(char **fields, size_t nfields, psa_drawing_t *p)
{
    REQ_MIN(48);    /* columns 1..47 */

    memset(p, 0, sizeof(*p));
    p->font_metrics[4] = 400;
    p->font_style[7] = 32;

    INT(p->drawing_type,   1,  0);
    STR(p->name,           2,  "");
    STR(p->key_text,       3,  "");
    DBL(p->x,              4,  0.0);
    DBL(p->width,          5,  0.0);
    DBL(p->y,              6,  0.0);
    DBL(p->height,         7,  0.0);
    DBL(p->z,              8,  0.0);
    DBL(p->depth,          9,  0.0);
    INT(p->fg_color,       10, -1);
    INT(p->bg_fill,        11, 0);
    INT(p->bg_color,       12, 16777215);
    INT(p->created_in_view,13, 2);
    INT(p->show_in_all_views,14,0);
    INT(p->word_wrap,      15, 0);
    INT(p->circular,       16, 0);
    DBL(p->start_x,        17, 0.0);
    DBL(p->start_y,        18, 0.0);
    DBL(p->start_z,        19, 0.0);
    DBL(p->end_x,          20, 0.0);
    DBL(p->end_y,          21, 0.0);
    DBL(p->end_z,          22, 0.0);
    STR(p->text,           23, "");
    INT(p->text_scale,     24, 2);
    INT(p->outline,        25, 1);
    INT(p->callout,        26, 0);
    for (int i = 0; i < 4;  i++) I64(p->font_metrics[i], 27 + i, 0);
    I64(p->font_metrics[4], 31, 400);
    for (int i = 0; i < 7;  i++) INT(p->font_style[i], 32 + i, 0);
    INT(p->font_style[7], 39, 32);
    STR(p->font_face,      40, "Arial");
    DBL(p->anchor_x,       41, 0.0);
    DBL(p->anchor_y,       42, 0.0);
    DBL(p->anchor_z,       43, 0.0);
    INT(p->center_text,    44, 1);
    INT(p->changed,        45, 0);
    INT(p->hide_when_printing,46,0);
    STR(p->custom_payload, 47, "");

    return 0;
}

/* ------------------------------------------------------------------------- */
/* Divider                                                                   */
/* ------------------------------------------------------------------------- */

int psa_parse_divider(char **fields, size_t nfields, psa_divider_t *p)
{
    REQ_MIN(18);    /* columns 0..17 (note: column 0 is indicator + data) */

    memset(p, 0, sizeof(*p));

    STR(p->id,             1,  "");
    DBL(p->x,              2,  0.0);
    DBL(p->width,          3,  0.0);
    DBL(p->y,              4,  0.0);
    DBL(p->height,         5,  0.0);
    DBL(p->z,              6,  0.0);
    DBL(p->depth,          7,  0.0);
    INT(p->color,          8,  0);
    STR(p->undef_text_1,   9,  "");
    STR(p->desc_text_1,    10, "");
    STR(p->desc_text_2,    11, "");
    STR(p->desc_text_3,    12, "");
    DBL(p->num_1,          13, 0.0);
    DBL(p->num_2,          14, 0.0);
    DBL(p->num_3,          15, 0.0);
    STR(p->undef_text_2,   16, "");
    STR(p->undef_text_3,   17, "");

    return 0;
}
