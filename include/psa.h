#ifndef PSA_H
#define PSA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/* ======================================================================== */
/* Return codes                                                             */
/* ======================================================================== */

#define PSA_OK          0
#define PSA_ERR_IO     -1
#define PSA_ERR_PARSE  -2
#define PSA_ERR_ABORT  -3

/* ------------------------------------------------------------------------- */
/* Numeric helpers (also used by internal parsers)                           */
/* ------------------------------------------------------------------------- */

int psa_parse_int(const char *s, int default_val);
double psa_parse_double(const char *s, double default_val);
int64_t psa_parse_int64(const char *s, int64_t default_val);

size_t psa_preprocess_line(const char *src, size_t len, char *dst, size_t dst_cap);
size_t psa_split_fields(char *buf, size_t len, char **fields, size_t max_fields);

/* ======================================================================== */
/* Record type enumeration                                                  */
/* ======================================================================== */

typedef enum {
    PSA_REC_PROJECT = 0,
    PSA_REC_PLANOGRAM,
    PSA_REC_FIXTURE,
    PSA_REC_PRODUCT,
    PSA_REC_POSITION,
    PSA_REC_PERFORMANCE,
    PSA_REC_SEGMENT,
    PSA_REC_DRAWING,
    PSA_REC_DIVIDER,
} psa_record_type_t;

/* ======================================================================== */
/* Extension-slot sizes (documented here for FFI consumers)                 */
/* ======================================================================== */

#define PSA_PROJECT_TEXT_SLOTS   50
#define PSA_PROJECT_NUM_SLOTS    50
#define PSA_PROJECT_FLAG_SLOTS   10

#define PSA_PLANOGRAM_TEXT_SLOTS 50
#define PSA_PLANOGRAM_NUM_SLOTS  50
#define PSA_PLANOGRAM_FLAG_SLOTS 10

#define PSA_FIXTURE_TEXT_SLOTS   30
#define PSA_FIXTURE_NUM_SLOTS    30
#define PSA_FIXTURE_FLAG_SLOTS   10

#define PSA_PRODUCT_TEXT_SLOTS   50
#define PSA_PRODUCT_NUM_SLOTS    50
#define PSA_PRODUCT_FLAG_SLOTS   10

#define PSA_POSITION_TEXT_SLOTS  30
#define PSA_POSITION_NUM_SLOTS   30
#define PSA_POSITION_FLAG_SLOTS  10

#define PSA_PERF_TEXT_SLOTS      50
#define PSA_PERF_NUM_SLOTS       50
#define PSA_PERF_FLAG_SLOTS      10

#define PSA_SEGMENT_TEXT_SLOTS   10
#define PSA_SEGMENT_NUM_SLOTS    10
#define PSA_SEGMENT_FLAG_SLOTS   10

/* ======================================================================== */
/* Record structs                                                           */
/*                                                                            */
/* All string pointers are owned by the parser and remain valid only for    */
/* the duration of the callback invocation.  Callers must copy (e.g. strdup)  */
/* if they need to persist the data.                                          */
/*                                                                            */
/* All fields are initialised to their documented defaults before parsing.  */
/* Required string fields are never NULL (they are empty string "" when     */
/* blank).  Optional string fields that are absent because an optional tail   */
/* section is not present are also left as empty string.                    */
/* ======================================================================== */

/* --- Project (indicator: "Project") ------------------------------------ */
typedef struct {
    const char *display_name;
    const char *key_text;
    int         primary_key;
    const char *layout_file;
    int         movement_period;
    double      case_multiple;
    double      days_supply;
    int         demand_cycle;
    double      peak_safety;
    double      backroom_stock;
    const char *peg_profile;
    int         measurement_mode;
    int         num_stores;

    int    merch_x[9];
    int    merch_y[9];
    int    merch_z[9];

    double demand[28];
    int    inv_model_opts[6];
    double num_ext[PSA_PROJECT_NUM_SLOTS];
    const char *text_ext[PSA_PROJECT_TEXT_SLOTS];
    int    flag_ext[PSA_PROJECT_FLAG_SLOTS];

    const char *notes;
    int         changed;
    int64_t     ext_db_keys[10];
    int         perf_override[4];
    const char *status;
    int         date_slots[8];
    const char *created_by;
    const char *modified_by;
    int         inv_mode;

    /* Optional tail (213..215) */
    const char *delivery_schedule;
    const char *custom_payload;
    int         family_key;
} psa_project_t;

/* --- Planogram (indicator: "Planogram") ------------------------------- */
typedef struct {
    const char *name;
    const char *key_text;
    double      width;
    double      height;
    double      depth;
    int         display_color;
    double      back_depth;
    int         draw_back;
    double      base_width;
    double      base_height;
    double      base_depth;
    int         draw_base;
    int         base_color;

    /* 14..21: notch/peg drawing controls (mixed int/double) */
    double    notch_peg[8];

    int         traffic_flow;
    int         auto_created;
    const char *shape_ref;
    const char *bitmap_ref;

    int    merch_x[9];
    int    merch_y[9];
    int    merch_z[9];

    double      combined_perf;
    int         store_count;
    double      notch_width;
    const char *text_ext[PSA_PLANOGRAM_TEXT_SLOTS];
    double      num_ext[PSA_PLANOGRAM_NUM_SLOTS];
    int         flag_ext[PSA_PLANOGRAM_FLAG_SLOTS];

    int         fill_pattern;
    const char *printable_segments;
    const char *source_file;
    int         changed;
    const char *layout_file;
    const char *notes;
    int64_t     ext_db_keys[10];
    int         source_type;
    const char *status[3];       /* 183..185 */
    int         date_slots[8];
    const char *created_by;
    const char *modified_by;
    const char *floor_bitmap_ref;
    double      door_transparency;
    double      floor_tile_width;
    double      floor_tile_depth;
    int         inv_model_opts[6];
    double      case_multiple;
    double      days_supply;
    int         demand_cycle;
    double      peak_safety;
    double      backroom_stock;
    double      demand[7];
    const char *delivery_schedule;
    const char *business_id;
    const char *department;
    const char *part_id;
    const char *gln_code;
    const char *custom_payload;
    const char *guid;
    const char *db_guid;
    const char *abbrev_name;
    const char *category;
    const char *subcategory;

    /* Optional A (229..232) */
    int         opt_source_code;
    const char *allocation_group;
    int         allocation_sequence;
    double      alloc_min_target;

    /* Optional B (233..254) – mixed; default -1 where present */
    double      alloc_max_target;
    int         split_ctrl;
    int         segment_ctrl;
    int         status_ctrl;
    int         score_ctrl;
    int         warning_count;
    int         error_count;
    const char *action_text;
    int         stage_limit;
    int         type_ref;
    int         model_ref;
    int         family_ref;
    int         version_ref;
    int         parent_ref;
    int         processing_ts;
    const char *server_text;
    int         final_status;

    /* 250..254: undocumented tail of optional section B */
    int         optb_250;
    int         optb_251;
    int         optb_252;
    int         optb_253;
    const char *optb_254;
} psa_planogram_t;

/* --- Fixture (indicator: "Fixture") ----------------------------------- */
typedef struct {
    int         type_code;
    const char *name;
    const char *key_text;
    double      x;
    double      width;
    double      y;
    double      height;
    double      z;
    double      depth;
    double      slope;
    double      angle;
    double      roll;
    int         color;
    const char *assembly;
    double      fixture_params[9]; /* 15..23: spacing/start/wall/curve/merch factors */
    int         collision_fixtures;
    int         collision_positions;
    int         can_obstruct;
    double      overhang[6];       /* 27..32 */
    int         default_merch_style;
    double      divider_dim[3];    /* 34..36 */
    int         combinable;
    double      grille_notch_peg[7]; /* 38..44 */
    const char *primary_label;
    const char *secondary_label;
    const char *shape_ref;
    const char *bitmap_ref;

    int    merch_x[9];
    int    merch_y[9];
    int    merch_z[9];

    const char *text_ext[PSA_FIXTURE_TEXT_SLOTS];
    double      num_ext[PSA_FIXTURE_NUM_SLOTS];
    int         flag_ext[PSA_FIXTURE_FLAG_SLOTS];

    int         location_id;
    int         fill_pattern;
    const char *model_file;
    double      weight_capacity;
    int         changed;
    int         divider_placement[3]; /* 151..153 */
    double      transparency;
    int         hide_when_printing;
    const char *product_assoc;
    const char *part_id;

    /* Optional tail (158..159) */
    int         hide_view_dims;
    const char *gln_code;

    /* Parser-injected context */
    int         planogram_key;
} psa_fixture_t;

/* --- Product (indicator: "Product") ----------------------------------- */
typedef struct {
    const char *upc;
    const char *business_id;
    const char *name;
    const char *key_text;
    double      width;
    double      height;
    double      depth;
    int         color;
    const char *abbrev_name;
    double      size;
    const char *uom;
    const char *manufacturer;
    const char *category;
    const char *supplier;
    int         inner_pack_qty;
    double      nesting_x;
    double      nesting_y;
    double      nesting_z;
    int         peg_hole_count;
    double      peg_hole_geom[9];   /* 20..28 */
    int         packaging_style;
    const char *peg_profile;
    double      finger_space_y;
    double      jumble_factor;
    double      price;
    double      case_cost;
    int         tax_code;
    double      unit_movement;
    double      share;
    double      case_multiple;
    double      days_supply;
    double      combined_perf;
    int         peg_span;
    int         min_units;
    int         max_units;
    const char *shape_ref;
    const char *bitmap_ref;

    /* Pack-size blocks (46..85) – treated as doubles */
    double tray[8];
    double case_pack[8];
    double display[8];
    double alternate[8];
    double loose[8];

    int    merch_xyz[27]; /* 86..112 */

    int         num_positions;
    const char *text_ext[PSA_PRODUCT_TEXT_SLOTS];
    double      num_ext[PSA_PRODUCT_NUM_SLOTS];
    int         flag_ext[PSA_PRODUCT_FLAG_SLOTS];

    double      squeeze_min_x;
    double      squeeze_max_x;
    double      squeeze_min_y;
    double      squeeze_max_y;
    double      squeeze_min_z;
    double      squeeze_max_z;
    int         fill_pattern;
    const char *model_file;
    const char *brand;
    const char *subcategory;
    double      weight;
    const char *planogram_alias;
    int         changed;
    double      front_overhang;
    double      finger_space_x;
    int64_t     ext_db_keys[10];
    const char *status;
    int         date_slots[8];
    const char *created_by;
    const char *modified_by;
    double      transparency;
    double      peak_safety;
    double      backroom_stock;
    const char *delivery_schedule;
    const char *part_id;
    int         authority_level;
    int         bitmap_unit_override;
    int         model_lookup_mode;
    int         default_merch_style;
    int         auto_model;
    const char *custom_payload;
    const char *db_guid;
    int         source_code;
    int64_t     technical_key;
} psa_product_t;

/* --- Position (indicator: "Position") ------------------------------- */
typedef struct {
    const char *upc;
    const char *business_id;
    const char *key_text;
    double      x;
    double      width;
    double      y;
    double      height;
    double      z;
    double      depth;
    double      slope;
    double      angle;
    double      roll;
    int         merch_style;
    int         h_facing;
    int         v_facing;
    int         d_facing;

    /* 17..20: X-capacity controls (orientation default 2) */
    int    x_cap[4];
    /* 21..24: Y-capacity controls (orientation default 4) */
    int    y_cap[4];
    /* 25..28: Z-capacity controls (reversed default 1, orientation default 2) */
    int    z_cap[4];

    int         orientation;
    double      jumble_x;
    double      jumble_y;
    double      jumble_z;
    double      merch_dim_x;
    double      merch_dim_y;
    double      merch_dim_z;
    double      full_dim_x;
    double      full_dim_y;
    double      full_dim_z;
    int         subunit_x;
    int         subunit_y;
    int         subunit_z;
    const char *peg_profile;
    int         manual_units;
    int         rank_x;
    int         rank_y;
    int         rank_z;
    int         peg_span;
    int         always_float;
    const char *primary_label;
    const char *secondary_label;

    int    merch_xyz[27]; /* 51..77 */

    const char *text_ext[PSA_POSITION_TEXT_SLOTS];
    double      num_ext[PSA_POSITION_NUM_SLOTS];
    int         flag_ext[PSA_POSITION_FLAG_SLOTS];

    int         target_space_x;
    int         target_space_y;
    int         target_space_z;
    double      target_val_x;
    double      target_val_y;
    double      target_val_z;
    int         location_id;
    int         changed;
    int         replenishment_min;
    int         replenishment_max;
    const char *shape_ref;
    const char *bitmap_ref;
    int         hide_when_printing;
    const char *part_id;
    int         bitmap_unit_override;
    int         auto_model;
    const char *custom_payload;
    int         x_cap_includes_units;
    int         y_cap_includes_units;

    /* Parser-injected context */
    int         planogram_key;
} psa_position_t;

/* --- Performance (indicator: "Performance") -------------------------- */
typedef struct {
    const char *upc;
    const char *business_id;
    const char *key_text;
    double      price;
    double      case_cost;
    int         tax_code;
    double      unit_movement;
    double      share;
    double      combined_perf;

    const char *text_ext[PSA_PERF_TEXT_SLOTS];
    double      num_ext[PSA_PERF_NUM_SLOTS];
    int         flag_ext[PSA_PERF_FLAG_SLOTS];

    int         changed;

    double      case_multiple;
    double      days_supply;
    double      peak_safety;
    double      backroom_stock;
    int         min_units;
    int         max_units;
    const char *delivery_schedule;
    int         replenishment_min;
    int         replenishment_max;
    int         assortment_rank;
    int         recommended_facings;
    const char *assortment_strategy;
    const char *assortment_tactic;
    const char *assortment_reason;
    const char *assortment_action;
    const char *part_id;
    const char *cluster_name;
    int         target_store_count;
    double      target_dist_percent;
    const char *assortment_note;

    const char *custom_payload;
    int         recommended_orientation;
    int         recommended_merch_style;
    int         ignore_recommendations;
    int         priority_code;
    const char *priority_desc;
    int         force_list;
    const char *planogram_reason;
    double      max_stage_reduction;
} psa_performance_t;

/* --- Segment (indicator: "Segment") ----------------------------------- */
typedef struct {
    const char *name;
    const char *key_text;
    double      x;
    double      width;
    double      y;
    double      height;
    double      z;
    double      depth;
    double      angle;
    double      x_offset;
    double      y_offset;

    int         door_flag;
    int         door_direction;
    const char *text_ext[PSA_SEGMENT_TEXT_SLOTS];
    double      num_ext[PSA_SEGMENT_NUM_SLOTS];
    int         flag_ext[PSA_SEGMENT_FLAG_SLOTS];
    double      frame_width;
    double      frame_height;
    int         changed;
    int         frame_color;
    int         frame_fill_pattern;
    const char *part_id;
    const char *gln_code;
    const char *custom_payload;

    /* Parser-injected context */
    int         planogram_key;
} psa_segment_t;

/* --- Drawing (indicator: "Drawing") ----------------------------------- */
typedef struct {
    int         drawing_type;
    const char *name;
    const char *key_text;
    double      x;
    double      width;
    double      y;
    double      height;
    double      z;
    double      depth;
    int         fg_color;
    int         bg_fill;
    int         bg_color;
    int         created_in_view;
    int         show_in_all_views;
    int         word_wrap;
    int         circular;
    double      start_x;
    double      start_y;
    double      start_z;
    double      end_x;
    double      end_y;
    double      end_z;
    const char *text;
    int         text_scale;
    int         outline;
    int         callout;
    int64_t     font_metrics[5];   /* 27..31 */
    int         font_style[8];       /* 32..39 */
    const char *font_face;
    double      anchor_x;
    double      anchor_y;
    double      anchor_z;
    int         center_text;
    int         changed;
    int         hide_when_printing;
    const char *custom_payload;
} psa_drawing_t;

/* --- Divider (indicator: "Divider") ----------------------------------- */
typedef struct {
    const char *id;
    double      x;
    double      width;
    double      y;
    double      height;
    double      z;
    double      depth;
    int         color;
    const char *undef_text_1;
    const char *desc_text_1;
    const char *desc_text_2;
    const char *desc_text_3;
    double      num_1;
    double      num_2;
    double      num_3;
    const char *undef_text_2;
    const char *undef_text_3;
} psa_divider_t;

/* ======================================================================== */
/* Tagged record (passed to callback)                                       */
/* ======================================================================== */

typedef struct {
    psa_record_type_t type;
    union {
        psa_project_t     project;
        psa_planogram_t   planogram;
        psa_fixture_t     fixture;
        psa_product_t     product;
        psa_position_t    position;
        psa_performance_t performance;
        psa_segment_t     segment;
        psa_drawing_t     drawing;
        psa_divider_t     divider;
    } rec;
} psa_record_t;

/* ======================================================================== */
/* Callbacks                                                                */
/* ======================================================================== */

/* Return 0 to continue parsing, any non-zero value to abort cleanly.       */
typedef int (*psa_record_callback)(psa_record_t *record, void *user_data);

/* ======================================================================== */
/* Core parsing API                                                         */
/* ======================================================================== */

/* Parse a PSA file.                                                        */
/*   path        – filesystem path to the .psa file.                         */
/*   out_header  – output: pointer to header line (line 0).  May be NULL.  */
/*   out_version – output: pointer to version line (line 1). May be NULL.  */
/*   cb          – callback invoked once for every successfully parsed     */
/*                 record.  Strings in the record are parser-owned and     */
/*                 valid only inside the callback.                           */
/*   user_data   – opaque pointer forwarded to the callback.                */
/*   errbuf      – buffer for error message on failure.  May be NULL.       */
/*   errbuf_size – capacity of errbuf.                                      */
/*                                                                            */
/* Returns PSA_OK on success, PSA_ERR_IO on I/O failure,                    */
/* PSA_ERR_PARSE on parse error, or PSA_ERR_ABORT if the callback returned  */
/* non-zero.                                                                */
int psa_parse_file(const char *path,
                   const char **out_header,
                   const char **out_version,
                   psa_record_callback cb,
                   void *user_data,
                   char *errbuf, size_t errbuf_size);

/* ======================================================================== */
/* JSON helpers (used by CLI, but also available to library consumers)     */
/* ======================================================================== */

/* Serialize a single record into JSON.  Returns number of bytes written,   */
/* or -1 if out_size is too small.  Does NOT write a trailing newline.      */
int psa_record_to_json(const psa_record_t *rec, char *out, size_t out_size);

/* Serialize file metadata into a JSON object.                              */
int psa_file_meta_to_json(const char *header, const char *version,
                          char *out, size_t out_size);

#ifdef __cplusplus
}
#endif

#endif /* PSA_H */
