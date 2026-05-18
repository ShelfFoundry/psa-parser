#include "psa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

/* Buffer helpers */
typedef struct { char *b; size_t p, c; } jb_t;
static int japp(jb_t *j, const char *s) {
    size_t n = strlen(s);
    if (j->p + n >= j->c) return -1;
    memcpy(j->b + j->p, s, n+1); j->p += n; return 0;
}
static int jch(jb_t *j, char c) {
    if (j->p + 1 >= j->c) return -1;
    j->b[j->p++] = c; j->b[j->p] = 0; return 0;
}
static int jint(jb_t *j, int v) { char t[32]; int n=snprintf(t,sizeof(t),"%d",v); return (n<0||(size_t)n>=sizeof(t))?-1:japp(j,t); }
static int ji64(jb_t *j, int64_t v) { char t[32]; int n=snprintf(t,sizeof(t),"%" PRId64,v); return (n<0||(size_t)n>=sizeof(t))?-1:japp(j,t); }
static int jdbl(jb_t *j, double v) { char t[64]; int n=snprintf(t,sizeof(t),"%.15g",v); return (n<0||(size_t)n>=sizeof(t))?-1:japp(j,t); }

static int jesc(jb_t *j, const char *s) {
    if (!s) return japp(j, "");
    for (const unsigned char *p = (const unsigned char *)s; *p; p++) {
        unsigned char c = *p;
        if (c == '"') { if (japp(j,"\\\"")<0) return -1; }
        else if (c == '\\') { if (japp(j,"\\\\")<0) return -1; }
        else if (c == '\b') { if (japp(j,"\\b")<0) return -1; }
        else if (c == '\f') { if (japp(j,"\\f")<0) return -1; }
        else if (c == '\n') { if (japp(j,"\\n")<0) return -1; }
        else if (c == '\r') { if (japp(j,"\\r")<0) return -1; }
        else if (c == '\t') { if (japp(j,"\\t")<0) return -1; }
        else if (c < 0x20) { char t[8]; snprintf(t,sizeof(t),"\\u%04x",c); if(japp(j,t)<0) return -1; }
        else { if (jch(j,(char)c)<0) return -1; }
    }
    return 0;
}

#define J(s) if(japp(j,(s))<0)return-1
#define K(k) J("\"");J(k);J("\":");
#define V(s) J("\"");jesc(j,(s));J("\"");
#define I(v) jint(j,(v))
#define D(v) jdbl(j,(v))
#define I6(v) ji64(j,(v))
#define AI(a,n) J("[");for(int _i=0;_i<(n);_i++){if(_i)J(",");I((a)[_i]);}J("]");
#define AD(a,n) J("[");for(int _i=0;_i<(n);_i++){if(_i)J(",");D((a)[_i]);}J("]");
#define AS(a,n) J("[");for(int _i=0;_i<(n);_i++){if(_i)J(",");V((a)[_i]);}J("]");
#define A6(a,n) J("[");for(int _i=0;_i<(n);_i++){if(_i)J(",");I6((a)[_i]);}J("]");

static int j_project(jb_t *j, const psa_project_t *p) {
J("{");K("type");V("Project");J(",");K("display_name");V(p->display_name);J(",");K("key_text");V(p->key_text);J(",");
K("primary_key");I(p->primary_key);J(",");K("layout_file");V(p->layout_file);J(",");K("movement_period");I(p->movement_period);J(",");
K("case_multiple");D(p->case_multiple);J(",");K("days_supply");D(p->days_supply);J(",");K("demand_cycle");I(p->demand_cycle);J(",");
K("peak_safety");D(p->peak_safety);J(",");K("backroom_stock");D(p->backroom_stock);J(",");K("peg_profile");V(p->peg_profile);J(",");
K("measurement_mode");I(p->measurement_mode);J(",");K("num_stores");I(p->num_stores);J(",");
K("merch_x");AI(p->merch_x,9);J(",");K("merch_y");AI(p->merch_y,9);J(",");K("merch_z");AI(p->merch_z,9);J(",");
K("demand");AD(p->demand,28);J(",");K("inv_model_opts");AI(p->inv_model_opts,6);J(",");
K("num_ext");AD(p->num_ext,PSA_PROJECT_NUM_SLOTS);J(",");K("text_ext");AS(p->text_ext,PSA_PROJECT_TEXT_SLOTS);J(",");
K("flag_ext");AI(p->flag_ext,PSA_PROJECT_FLAG_SLOTS);J(",");
K("notes");V(p->notes);J(",");K("changed");I(p->changed);J(",");K("ext_db_keys");A6(p->ext_db_keys,10);J(",");
K("perf_override");AI(p->perf_override,4);J(",");K("status");V(p->status);J(",");K("date_slots");AI(p->date_slots,8);J(",");
K("created_by");V(p->created_by);J(",");K("modified_by");V(p->modified_by);J(",");K("inv_mode");I(p->inv_mode);J(",");
K("delivery_schedule");V(p->delivery_schedule);J(",");K("custom_payload");V(p->custom_payload);J(",");K("family_key");I(p->family_key);
J("}");return 0;
}

static int j_planogram(jb_t *j, const psa_planogram_t *p) {
J("{");K("type");V("Planogram");J(",");K("name");V(p->name);J(",");K("key_text");V(p->key_text);J(",");
K("width");D(p->width);J(",");K("height");D(p->height);J(",");K("depth");D(p->depth);J(",");
K("display_color");I(p->display_color);J(",");K("back_depth");D(p->back_depth);J(",");K("draw_back");I(p->draw_back);J(",");
K("base_width");D(p->base_width);J(",");K("base_height");D(p->base_height);J(",");K("base_depth");D(p->base_depth);J(",");
K("draw_base");I(p->draw_base);J(",");K("base_color");I(p->base_color);J(",");
K("notch_peg");AD(p->notch_peg,8);J(",");K("traffic_flow");I(p->traffic_flow);J(",");K("auto_created");I(p->auto_created);J(",");
K("shape_ref");V(p->shape_ref);J(",");K("bitmap_ref");V(p->bitmap_ref);J(",");
K("merch_x");AI(p->merch_x,9);J(",");K("merch_y");AI(p->merch_y,9);J(",");K("merch_z");AI(p->merch_z,9);J(",");
K("combined_perf");D(p->combined_perf);J(",");K("store_count");I(p->store_count);J(",");K("notch_width");D(p->notch_width);J(",");
K("text_ext");AS(p->text_ext,PSA_PLANOGRAM_TEXT_SLOTS);J(",");K("num_ext");AD(p->num_ext,PSA_PLANOGRAM_NUM_SLOTS);J(",");
K("flag_ext");AI(p->flag_ext,PSA_PLANOGRAM_FLAG_SLOTS);J(",");
K("fill_pattern");I(p->fill_pattern);J(",");K("printable_segments");V(p->printable_segments);J(",");K("source_file");V(p->source_file);J(",");
K("changed");I(p->changed);J(",");K("layout_file");V(p->layout_file);J(",");K("notes");V(p->notes);J(",");
K("ext_db_keys");A6(p->ext_db_keys,10);J(",");K("source_type");I(p->source_type);J(",");
K("status");AS(p->status,3);J(",");K("date_slots");AI(p->date_slots,8);J(",");
K("created_by");V(p->created_by);J(",");K("modified_by");V(p->modified_by);J(",");K("floor_bitmap_ref");V(p->floor_bitmap_ref);J(",");
K("door_transparency");D(p->door_transparency);J(",");K("floor_tile_width");D(p->floor_tile_width);J(",");K("floor_tile_depth");D(p->floor_tile_depth);J(",");
K("inv_model_opts");AI(p->inv_model_opts,6);J(",");K("case_multiple");D(p->case_multiple);J(",");K("days_supply");D(p->days_supply);J(",");
K("demand_cycle");I(p->demand_cycle);J(",");K("peak_safety");D(p->peak_safety);J(",");K("backroom_stock");D(p->backroom_stock);J(",");
K("demand");AD(p->demand,7);J(",");K("delivery_schedule");V(p->delivery_schedule);J(",");K("business_id");V(p->business_id);J(",");
K("department");V(p->department);J(",");K("part_id");V(p->part_id);J(",");K("gln_code");V(p->gln_code);J(",");
K("custom_payload");V(p->custom_payload);J(",");K("guid");V(p->guid);J(",");K("db_guid");V(p->db_guid);J(",");
K("abbrev_name");V(p->abbrev_name);J(",");K("category");V(p->category);J(",");K("subcategory");V(p->subcategory);J(",");
K("opt_source_code");I(p->opt_source_code);J(",");K("allocation_group");V(p->allocation_group);J(",");K("allocation_sequence");I(p->allocation_sequence);J(",");K("alloc_min_target");D(p->alloc_min_target);J(",");
K("alloc_max_target");D(p->alloc_max_target);J(",");K("split_ctrl");I(p->split_ctrl);J(",");K("segment_ctrl");I(p->segment_ctrl);J(",");K("status_ctrl");I(p->status_ctrl);J(",");K("score_ctrl");I(p->score_ctrl);J(",");
K("warning_count");I(p->warning_count);J(",");K("error_count");I(p->error_count);J(",");K("action_text");V(p->action_text);J(",");K("stage_limit");I(p->stage_limit);J(",");
K("type_ref");I(p->type_ref);J(",");K("model_ref");I(p->model_ref);J(",");K("family_ref");I(p->family_ref);J(",");K("version_ref");I(p->version_ref);J(",");K("parent_ref");I(p->parent_ref);J(",");
K("processing_ts");I(p->processing_ts);J(",");K("server_text");V(p->server_text);J(",");K("final_status");I(p->final_status);
J("}");return 0;
}

static int j_fixture(jb_t *j, const psa_fixture_t *p) {
J("{");K("type");V("Fixture");J(",");K("planogram_key");I(p->planogram_key);J(",");
K("type_code");I(p->type_code);J(",");K("name");V(p->name);J(",");K("key_text");V(p->key_text);J(",");
K("x");D(p->x);J(",");K("width");D(p->width);J(",");K("y");D(p->y);J(",");K("height");D(p->height);J(",");K("z");D(p->z);J(",");K("depth");D(p->depth);J(",");
K("slope");D(p->slope);J(",");K("angle");D(p->angle);J(",");K("roll");D(p->roll);J(",");K("color");I(p->color);J(",");K("assembly");V(p->assembly);J(",");
K("fixture_params");AD(p->fixture_params,9);J(",");K("collision_fixtures");I(p->collision_fixtures);J(",");K("collision_positions");I(p->collision_positions);J(",");K("can_obstruct");I(p->can_obstruct);J(",");
K("overhang");AD(p->overhang,6);J(",");K("default_merch_style");I(p->default_merch_style);J(",");K("divider_dim");AD(p->divider_dim,3);J(",");K("combinable");I(p->combinable);J(",");
K("grille_notch_peg");AD(p->grille_notch_peg,7);J(",");K("primary_label");V(p->primary_label);J(",");K("secondary_label");V(p->secondary_label);J(",");K("shape_ref");V(p->shape_ref);J(",");K("bitmap_ref");V(p->bitmap_ref);J(",");
K("merch_x");AI(p->merch_x,9);J(",");K("merch_y");AI(p->merch_y,9);J(",");K("merch_z");AI(p->merch_z,9);J(",");
K("text_ext");AS(p->text_ext,PSA_FIXTURE_TEXT_SLOTS);J(",");K("num_ext");AD(p->num_ext,PSA_FIXTURE_NUM_SLOTS);J(",");K("flag_ext");AI(p->flag_ext,PSA_FIXTURE_FLAG_SLOTS);J(",");
K("location_id");I(p->location_id);J(",");K("fill_pattern");I(p->fill_pattern);J(",");K("model_file");V(p->model_file);J(",");K("weight_capacity");D(p->weight_capacity);J(",");K("changed");I(p->changed);J(",");
K("divider_placement");AI(p->divider_placement,3);J(",");K("transparency");D(p->transparency);J(",");K("hide_when_printing");I(p->hide_when_printing);J(",");K("product_assoc");V(p->product_assoc);J(",");K("part_id");V(p->part_id);J(",");
K("hide_view_dims");I(p->hide_view_dims);J(",");K("gln_code");V(p->gln_code);
J("}");return 0;
}

static int j_product(jb_t *j, const psa_product_t *p) {
J("{");K("type");V("Product");J(",");K("upc");V(p->upc);J(",");K("business_id");V(p->business_id);J(",");K("name");V(p->name);J(",");K("key_text");V(p->key_text);J(",");
K("width");D(p->width);J(",");K("height");D(p->height);J(",");K("depth");D(p->depth);J(",");K("color");I(p->color);J(",");K("abbrev_name");V(p->abbrev_name);J(",");K("size");D(p->size);J(",");
K("uom");V(p->uom);J(",");K("manufacturer");V(p->manufacturer);J(",");K("category");V(p->category);J(",");K("supplier");V(p->supplier);J(",");K("inner_pack_qty");I(p->inner_pack_qty);J(",");
K("nesting_x");D(p->nesting_x);J(",");K("nesting_y");D(p->nesting_y);J(",");K("nesting_z");D(p->nesting_z);J(",");K("peg_hole_count");I(p->peg_hole_count);J(",");
K("peg_hole_geom");AD(p->peg_hole_geom,9);J(",");K("packaging_style");I(p->packaging_style);J(",");K("peg_profile");V(p->peg_profile);J(",");
K("finger_space_y");D(p->finger_space_y);J(",");K("jumble_factor");D(p->jumble_factor);J(",");K("price");D(p->price);J(",");K("case_cost");D(p->case_cost);J(",");K("tax_code");I(p->tax_code);J(",");
K("unit_movement");D(p->unit_movement);J(",");K("share");D(p->share);J(",");K("case_multiple");D(p->case_multiple);J(",");K("days_supply");D(p->days_supply);J(",");K("combined_perf");D(p->combined_perf);J(",");
K("peg_span");I(p->peg_span);J(",");K("min_units");I(p->min_units);J(",");K("max_units");I(p->max_units);J(",");K("shape_ref");V(p->shape_ref);J(",");K("bitmap_ref");V(p->bitmap_ref);J(",");
K("tray");AD(p->tray,8);J(",");K("case_pack");AD(p->case_pack,8);J(",");K("display");AD(p->display,8);J(",");K("alternate");AD(p->alternate,8);J(",");K("loose");AD(p->loose,8);J(",");
K("merch_xyz");AI(p->merch_xyz,27);J(",");K("num_positions");I(p->num_positions);J(",");
K("text_ext");AS(p->text_ext,PSA_PRODUCT_TEXT_SLOTS);J(",");K("num_ext");AD(p->num_ext,PSA_PRODUCT_NUM_SLOTS);J(",");K("flag_ext");AI(p->flag_ext,PSA_PRODUCT_FLAG_SLOTS);J(",");
K("squeeze_min_x");D(p->squeeze_min_x);J(",");K("squeeze_max_x");D(p->squeeze_max_x);J(",");K("squeeze_min_y");D(p->squeeze_min_y);J(",");K("squeeze_max_y");D(p->squeeze_max_y);J(",");K("squeeze_min_z");D(p->squeeze_min_z);J(",");K("squeeze_max_z");D(p->squeeze_max_z);J(",");
K("fill_pattern");I(p->fill_pattern);J(",");K("model_file");V(p->model_file);J(",");K("brand");V(p->brand);J(",");K("subcategory");V(p->subcategory);J(",");K("weight");D(p->weight);J(",");K("planogram_alias");V(p->planogram_alias);J(",");
K("changed");I(p->changed);J(",");K("front_overhang");D(p->front_overhang);J(",");K("finger_space_x");D(p->finger_space_x);J(",");K("ext_db_keys");A6(p->ext_db_keys,10);J(",");K("status");V(p->status);J(",");
K("date_slots");AI(p->date_slots,8);J(",");K("created_by");V(p->created_by);J(",");K("modified_by");V(p->modified_by);J(",");
K("transparency");D(p->transparency);J(",");K("peak_safety");D(p->peak_safety);J(",");K("backroom_stock");D(p->backroom_stock);J(",");K("delivery_schedule");V(p->delivery_schedule);J(",");K("part_id");V(p->part_id);J(",");
K("authority_level");I(p->authority_level);J(",");K("bitmap_unit_override");I(p->bitmap_unit_override);J(",");K("model_lookup_mode");I(p->model_lookup_mode);J(",");K("default_merch_style");I(p->default_merch_style);J(",");K("auto_model");I(p->auto_model);J(",");
K("custom_payload");V(p->custom_payload);J(",");K("db_guid");V(p->db_guid);J(",");K("source_code");I(p->source_code);J(",");K("technical_key");I6(p->technical_key);
J("}");return 0;
}

static int j_position(jb_t *j, const psa_position_t *p) {
J("{");K("type");V("Position");J(",");K("planogram_key");I(p->planogram_key);J(",");
K("upc");V(p->upc);J(",");K("business_id");V(p->business_id);J(",");K("key_text");V(p->key_text);J(",");
K("x");D(p->x);J(",");K("width");D(p->width);J(",");K("y");D(p->y);J(",");K("height");D(p->height);J(",");K("z");D(p->z);J(",");K("depth");D(p->depth);J(",");
K("slope");D(p->slope);J(",");K("angle");D(p->angle);J(",");K("roll");D(p->roll);J(",");K("merch_style");I(p->merch_style);J(",");
K("h_facing");I(p->h_facing);J(",");K("v_facing");I(p->v_facing);J(",");K("d_facing");I(p->d_facing);J(",");
K("x_cap");AI(p->x_cap,4);J(",");K("y_cap");AI(p->y_cap,4);J(",");K("z_cap");AI(p->z_cap,4);J(",");
K("orientation");I(p->orientation);J(",");
K("jumble_x");D(p->jumble_x);J(",");K("jumble_y");D(p->jumble_y);J(",");K("jumble_z");D(p->jumble_z);J(",");
K("merch_dim_x");D(p->merch_dim_x);J(",");K("merch_dim_y");D(p->merch_dim_y);J(",");K("merch_dim_z");D(p->merch_dim_z);J(",");
K("full_dim_x");D(p->full_dim_x);J(",");K("full_dim_y");D(p->full_dim_y);J(",");K("full_dim_z");D(p->full_dim_z);J(",");
K("subunit_x");I(p->subunit_x);J(",");K("subunit_y");I(p->subunit_y);J(",");K("subunit_z");I(p->subunit_z);J(",");K("peg_profile");V(p->peg_profile);J(",");
K("manual_units");I(p->manual_units);J(",");K("rank_x");I(p->rank_x);J(",");K("rank_y");I(p->rank_y);J(",");K("rank_z");I(p->rank_z);J(",");K("peg_span");I(p->peg_span);J(",");K("always_float");I(p->always_float);J(",");
K("primary_label");V(p->primary_label);J(",");K("secondary_label");V(p->secondary_label);J(",");
K("merch_xyz");AI(p->merch_xyz,27);J(",");
K("text_ext");AS(p->text_ext,PSA_POSITION_TEXT_SLOTS);J(",");K("num_ext");AD(p->num_ext,PSA_POSITION_NUM_SLOTS);J(",");K("flag_ext");AI(p->flag_ext,PSA_POSITION_FLAG_SLOTS);J(",");
K("target_space_x");I(p->target_space_x);J(",");K("target_space_y");I(p->target_space_y);J(",");K("target_space_z");I(p->target_space_z);J(",");
K("target_val_x");D(p->target_val_x);J(",");K("target_val_y");D(p->target_val_y);J(",");K("target_val_z");D(p->target_val_z);J(",");
K("location_id");I(p->location_id);J(",");K("changed");I(p->changed);J(",");K("replenishment_min");I(p->replenishment_min);J(",");K("replenishment_max");I(p->replenishment_max);J(",");
K("shape_ref");V(p->shape_ref);J(",");K("bitmap_ref");V(p->bitmap_ref);J(",");K("hide_when_printing");I(p->hide_when_printing);J(",");K("part_id");V(p->part_id);J(",");
K("bitmap_unit_override");I(p->bitmap_unit_override);J(",");K("auto_model");I(p->auto_model);J(",");K("custom_payload");V(p->custom_payload);J(",");
K("x_cap_includes_units");I(p->x_cap_includes_units);J(",");K("y_cap_includes_units");I(p->y_cap_includes_units);
J("}");return 0;
}

static int j_performance(jb_t *j, const psa_performance_t *p) {
J("{");K("type");V("Performance");J(",");K("upc");V(p->upc);J(",");K("business_id");V(p->business_id);J(",");K("key_text");V(p->key_text);J(",");
K("price");D(p->price);J(",");K("case_cost");D(p->case_cost);J(",");K("tax_code");I(p->tax_code);J(",");K("unit_movement");D(p->unit_movement);J(",");K("share");D(p->share);J(",");K("combined_perf");D(p->combined_perf);J(",");
K("text_ext");AS(p->text_ext,PSA_PERF_TEXT_SLOTS);J(",");K("num_ext");AD(p->num_ext,PSA_PERF_NUM_SLOTS);J(",");K("flag_ext");AI(p->flag_ext,PSA_PERF_FLAG_SLOTS);J(",");
K("changed");I(p->changed);J(",");K("case_multiple");D(p->case_multiple);J(",");K("days_supply");D(p->days_supply);J(",");K("peak_safety");D(p->peak_safety);J(",");K("backroom_stock");D(p->backroom_stock);J(",");
K("min_units");I(p->min_units);J(",");K("max_units");I(p->max_units);J(",");K("delivery_schedule");V(p->delivery_schedule);J(",");
K("replenishment_min");I(p->replenishment_min);J(",");K("replenishment_max");I(p->replenishment_max);J(",");K("assortment_rank");I(p->assortment_rank);J(",");K("recommended_facings");I(p->recommended_facings);J(",");
K("assortment_strategy");V(p->assortment_strategy);J(",");K("assortment_tactic");V(p->assortment_tactic);J(",");K("assortment_reason");V(p->assortment_reason);J(",");K("assortment_action");V(p->assortment_action);J(",");
K("part_id");V(p->part_id);J(",");K("cluster_name");V(p->cluster_name);J(",");K("target_store_count");I(p->target_store_count);J(",");K("target_dist_percent");D(p->target_dist_percent);J(",");K("assortment_note");V(p->assortment_note);J(",");
K("custom_payload");V(p->custom_payload);J(",");K("recommended_orientation");I(p->recommended_orientation);J(",");K("recommended_merch_style");I(p->recommended_merch_style);J(",");
K("ignore_recommendations");I(p->ignore_recommendations);J(",");K("priority_code");I(p->priority_code);J(",");K("priority_desc");V(p->priority_desc);J(",");K("force_list");I(p->force_list);J(",");K("planogram_reason");V(p->planogram_reason);J(",");K("max_stage_reduction");D(p->max_stage_reduction);
J("}");return 0;
}

static int j_segment(jb_t *j, const psa_segment_t *p) {
J("{");K("type");V("Segment");J(",");K("planogram_key");I(p->planogram_key);J(",");
K("name");V(p->name);J(",");K("key_text");V(p->key_text);J(",");K("x");D(p->x);J(",");K("width");D(p->width);J(",");K("y");D(p->y);J(",");K("height");D(p->height);J(",");K("z");D(p->z);J(",");K("depth");D(p->depth);J(",");
K("angle");D(p->angle);J(",");K("x_offset");D(p->x_offset);J(",");K("y_offset");D(p->y_offset);J(",");
K("door_flag");I(p->door_flag);J(",");K("door_direction");I(p->door_direction);J(",");
K("text_ext");AS(p->text_ext,PSA_SEGMENT_TEXT_SLOTS);J(",");K("num_ext");AD(p->num_ext,PSA_SEGMENT_NUM_SLOTS);J(",");K("flag_ext");AI(p->flag_ext,PSA_SEGMENT_FLAG_SLOTS);J(",");
K("frame_width");D(p->frame_width);J(",");K("frame_height");D(p->frame_height);J(",");K("changed");I(p->changed);J(",");K("frame_color");I(p->frame_color);J(",");K("frame_fill_pattern");I(p->frame_fill_pattern);J(",");
K("part_id");V(p->part_id);J(",");K("gln_code");V(p->gln_code);J(",");K("custom_payload");V(p->custom_payload);
J("}");return 0;
}

static int j_drawing(jb_t *j, const psa_drawing_t *p) {
J("{");K("type");V("Drawing");J(",");K("drawing_type");I(p->drawing_type);J(",");K("name");V(p->name);J(",");K("key_text");V(p->key_text);J(",");
K("x");D(p->x);J(",");K("width");D(p->width);J(",");K("y");D(p->y);J(",");K("height");D(p->height);J(",");K("z");D(p->z);J(",");K("depth");D(p->depth);J(",");
K("fg_color");I(p->fg_color);J(",");K("bg_fill");I(p->bg_fill);J(",");K("bg_color");I(p->bg_color);J(",");K("created_in_view");I(p->created_in_view);J(",");K("show_in_all_views");I(p->show_in_all_views);J(",");
K("word_wrap");I(p->word_wrap);J(",");K("circular");I(p->circular);J(",");
K("start_x");D(p->start_x);J(",");K("start_y");D(p->start_y);J(",");K("start_z");D(p->start_z);J(",");K("end_x");D(p->end_x);J(",");K("end_y");D(p->end_y);J(",");K("end_z");D(p->end_z);J(",");
K("text");V(p->text);J(",");K("text_scale");I(p->text_scale);J(",");K("outline");I(p->outline);J(",");K("callout");I(p->callout);J(",");
K("font_metrics");A6(p->font_metrics,5);J(",");K("font_style");AI(p->font_style,8);J(",");K("font_face");V(p->font_face);J(",");
K("anchor_x");D(p->anchor_x);J(",");K("anchor_y");D(p->anchor_y);J(",");K("anchor_z");D(p->anchor_z);J(",");
K("center_text");I(p->center_text);J(",");K("changed");I(p->changed);J(",");K("hide_when_printing");I(p->hide_when_printing);J(",");K("custom_payload");V(p->custom_payload);
J("}");return 0;
}

static int j_divider(jb_t *j, const psa_divider_t *p) {
J("{");K("type");V("Divider");J(",");K("id");V(p->id);J(",");K("x");D(p->x);J(",");K("width");D(p->width);J(",");K("y");D(p->y);J(",");K("height");D(p->height);J(",");K("z");D(p->z);J(",");K("depth");D(p->depth);J(",");K("color");I(p->color);J(",");
K("undef_text_1");V(p->undef_text_1);J(",");K("desc_text_1");V(p->desc_text_1);J(",");K("desc_text_2");V(p->desc_text_2);J(",");K("desc_text_3");V(p->desc_text_3);J(",");
K("num_1");D(p->num_1);J(",");K("num_2");D(p->num_2);J(",");K("num_3");D(p->num_3);J(",");
K("undef_text_2");V(p->undef_text_2);J(",");K("undef_text_3");V(p->undef_text_3);
J("}");return 0;
}

int psa_file_meta_to_json(const char *header, const char *version,
                          char *out, size_t out_size)
{
    jb_t jb = { out, 0, out_size };
    jb_t *j = &jb;
    J("{");K("header");V(header);J(",");K("version");V(version);J("}");
    return (int)jb.p;
}

int psa_record_to_json(const psa_record_t *rec, char *out, size_t out_size)
{
    jb_t j = { out, 0, out_size };
    int rc;
    switch (rec->type) {
        case PSA_REC_PROJECT:     rc = j_project(&j, &rec->rec.project); break;
        case PSA_REC_PLANOGRAM:   rc = j_planogram(&j, &rec->rec.planogram); break;
        case PSA_REC_FIXTURE:     rc = j_fixture(&j, &rec->rec.fixture); break;
        case PSA_REC_PRODUCT:     rc = j_product(&j, &rec->rec.product); break;
        case PSA_REC_POSITION:    rc = j_position(&j, &rec->rec.position); break;
        case PSA_REC_PERFORMANCE: rc = j_performance(&j, &rec->rec.performance); break;
        case PSA_REC_SEGMENT:     rc = j_segment(&j, &rec->rec.segment); break;
        case PSA_REC_DRAWING:     rc = j_drawing(&j, &rec->rec.drawing); break;
        case PSA_REC_DIVIDER:     rc = j_divider(&j, &rec->rec.divider); break;
        default: return -1;
    }
    if (rc < 0) return -1;
    return (int)j.p;
}

typedef struct {
    char *buf;
    size_t len;
    size_t cap;
    int count;
} json_bucket_t;

typedef struct {
    json_bucket_t buckets[9];
    int failed;
} doc_accum_t;

static int bucket_append(json_bucket_t *b, const char *s, size_t n)
{
    size_t need = b->len + n + 1;
    if (need > b->cap) {
        size_t new_cap = b->cap ? b->cap : 1024;
        while (new_cap < need)
            new_cap *= 2;
        char *new_buf = realloc(b->buf, new_cap);
        if (!new_buf)
            return -1;
        b->buf = new_buf;
        b->cap = new_cap;
    }

    memcpy(b->buf + b->len, s, n);
    b->len += n;
    b->buf[b->len] = '\0';
    return 0;
}

static int document_cb(psa_record_t *rec, void *user_data)
{
    doc_accum_t *acc = (doc_accum_t *)user_data;
    json_bucket_t *b;
    char *tmp;
    size_t cap = 4096;
    int n;

    if ((int)rec->type < 0 || (int)rec->type >= 9) {
        acc->failed = 1;
        return 1;
    }

    tmp = malloc(cap);
    if (!tmp) {
        acc->failed = 1;
        return 1;
    }

    for (;;) {
        n = psa_record_to_json(rec, tmp, cap);
        if (n >= 0)
            break;
        if (cap >= 8 * 1024 * 1024) {
            free(tmp);
            acc->failed = 1;
            return 1;
        }
        cap *= 2;
        char *next = realloc(tmp, cap);
        if (!next) {
            free(tmp);
            acc->failed = 1;
            return 1;
        }
        tmp = next;
    }

    b = &acc->buckets[(int)rec->type];
    if (b->count > 0 && bucket_append(b, ",", 1) < 0) {
        free(tmp);
        acc->failed = 1;
        return 1;
    }
    if (bucket_append(b, tmp, (size_t)n) < 0) {
        free(tmp);
        acc->failed = 1;
        return 1;
    }

    b->count++;
    free(tmp);
    return 0;
}

int psa_parse_file_to_json_document(const char *path,
                                    char *out, size_t out_size,
                                    char *errbuf, size_t errbuf_size)
{
    doc_accum_t acc = {0};
    const char *header = NULL;
    const char *version = NULL;
    int rc;
    int out_n;
    jb_t jb = { out, 0, out_size };
    jb_t *j = &jb;
    const char *array_names[9] = {
        "projects", "planograms", "fixtures", "products",
        "positions", "performances", "segments", "drawings", "dividers"
    };

    rc = psa_parse_file(path, &header, &version, document_cb, &acc, errbuf, errbuf_size);
    if (rc != PSA_OK) {
        if (rc == PSA_ERR_ABORT && acc.failed && errbuf && errbuf_size > 0 && errbuf[0] == '\0')
            snprintf(errbuf, errbuf_size, "Failed to build JSON document");
        free((void *)header);
        free((void *)version);
        for (int i = 0; i < 9; i++)
            free(acc.buckets[i].buf);
        return rc;
    }

    if (japp(j, "{") < 0) { out_n = -1; goto cleanup; }
    if (japp(j, "\"header\":\"") < 0) { out_n = -1; goto cleanup; }
    if (jesc(j, header ? header : "") < 0) { out_n = -1; goto cleanup; }
    if (japp(j, "\",\"version\":\"") < 0) { out_n = -1; goto cleanup; }
    if (jesc(j, version ? version : "") < 0) { out_n = -1; goto cleanup; }
    if (japp(j, "\"") < 0) { out_n = -1; goto cleanup; }

    for (int i = 0; i < 9; i++) {
        if (japp(j, ",\"") < 0) { out_n = -1; goto cleanup; }
        if (japp(j, array_names[i]) < 0) { out_n = -1; goto cleanup; }
        if (japp(j, "\":[") < 0) { out_n = -1; goto cleanup; }
        if (acc.buckets[i].len > 0 && japp(j, acc.buckets[i].buf) < 0) { out_n = -1; goto cleanup; }
        if (japp(j, "]") < 0) { out_n = -1; goto cleanup; }
    }

    if (japp(j, "}") < 0) { out_n = -1; goto cleanup; }
    out_n = (int)jb.p;

cleanup:
    free((void *)header);
    free((void *)version);
    for (int i = 0; i < 9; i++)
        free(acc.buckets[i].buf);
    return out_n;
}
