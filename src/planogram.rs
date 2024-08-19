use crate::{alloc::Alloc, date::Date, inventory::Inventory, merch::{Merch, MerchDetails}, notch::Notch};

pub struct Planogram {
    pub id: String,
    pub name: String,
    pub abbrevated_name: String,
    pub key: String,
    pub category: String,
    pub subcategory: String,
    pub use_as_subplanogram: i32,
    pub source: i32,
    pub width: f32,
    pub height: f32,
    pub depth: f32,
    pub color: i32,
    pub back_depth: f32,
    pub draw_back: i32,
    pub base_width: f32,
    pub base_height: f32,
    pub base_depth: f32,
    pub draw_base: i32,
    pub base_color: i32,
    pub notch: Notch,
    pub draw_pegs: i32,
    pub draw_peg_holes: i32,
    pub traffic_flow: i32,
    pub auto_created: i32,
    pub shape_id: String,
    pub bit_map_id: String,
    pub merch: Merch,
    pub number_of_stores: f32,
    pub values: [f32;50],
    pub descriptions: [String;50],
    pub flags: [i32;10],
    pub fill_pattern: i32,
    pub segments_to_print: String,
    pub file_name: String,
    pub source_file_type: i32,
    pub changed: i32,
    pub layout_file_name: String,
    pub notes: String,
    pub date: Date,
    pub statuses: [String;3],
    pub created_by: String,
    pub modified_by: String,
    pub floor_bitmap_id: String,
    pub door_transparency: f32,
    pub floor_tile_width: f32,
    pub floor_tile_depth: f32,
    pub inventory: Inventory,
    pub case_multiplier: f32,
    pub days_supply: f32,
    pub demand_cycle_length: i32,
    pub peak_saftey_factor: f32,
    pub backroom_stock: f32,
    pub demands: [f32;7],
    pub delivery_schedule: String,
    pub department: String,
    pub part_id: String,
    pub gln: String,
    pub custom_data: String,
    pub can_segment: i32,
    pub can_split: i32,
    pub checksums: [String;5],
    pub number_of_fixtures: i32,
    pub number_of_segments: i32,
    pub number_of_drawings: i32,
    pub warnings_count: i32,
    pub combined_perforamnce_index: f32,
    pub warning: String,
    pub warning_number: i32,
    pub number_of_recommendations: i32,
    pub model_file_name: String,
    pub alloc: Alloc,
    pub pr_status: i32,
    pub pg_status: i32,
    pub pg_core_percent: i32,
    pub pg_core_note: String,
    pub pg_warnings_count: i32,
    pub pg_errors_count: i32,
    pub pg_action_list: String,
    pub pg_max_stage_reduce: f32,
    pub pg_max_stage_fillout: f32,
    pub pg_type: i32,
    pub pg_server_name: String,
    pub movement_peroid: i32,
}

impl Planogram {
    pub fn new(line: String) -> Self {
        let values:Vec<&str> = line.split(',').into_iter().collect();

        let mut name = String::from("Planogram");
        if values[1] != "" {
            name = values[1].to_string();
        }

        let merch = Merch {
            x: MerchDetails {
                min: values[26].parse().unwrap_or(-1),
                max: values[27].parse().unwrap_or(-1),
                uprights: values[28].parse().unwrap_or(-1),
                caps: values[29].parse().unwrap_or(-1),
                placement: values[30].parse().unwrap_or(0),
                number: values[31].parse().unwrap_or(0),
                size: values[32].parse().unwrap_or(0),
                direction: values[33].parse().unwrap_or(-1),
                squeeze: values[34].parse().unwrap_or(-1),
            },
            y: MerchDetails {
                min: values[35].parse().unwrap_or(-1),
                max: values[36].parse().unwrap_or(-1),
                uprights: values[37].parse().unwrap_or(-1),
                caps: values[38].parse().unwrap_or(-1),
                placement: values[39].parse().unwrap_or(0),
                number: values[40].parse().unwrap_or(0),
                size: values[41].parse().unwrap_or(0),
                direction: values[42].parse().unwrap_or(-1),
                squeeze: values[43].parse().unwrap_or(-1),
            },
            z: MerchDetails {
                min: values[44].parse().unwrap_or(-1),
                max: values[45].parse().unwrap_or(-1),
                uprights: values[46].parse().unwrap_or(-1),
                caps: values[47].parse().unwrap_or(-1),
                placement: values[48].parse().unwrap_or(0),
                number: values[49].parse().unwrap_or(0),
                size: values[50].parse().unwrap_or(0),
                direction: values[51].parse().unwrap_or(-1),
                squeeze: values[52].parse().unwrap_or(-1),
            },
        };

        let inventory = Inventory {
            manual: values[200].parse().unwrap_or(0),
            case_multiplier: values[201].parse().unwrap_or(0),
            days_supply: values[202].parse().unwrap_or(0),
            peak: values[203].parse().unwrap_or(0),
            min_units: values[204].parse().unwrap_or(0),
            max_units: values[205].parse().unwrap_or(0),
        };

        let notch = Notch {
            draw_notches: values[14].parse().unwrap_or(0),
            offset: values[15].parse().unwrap_or(0.0),
            spacing: values[16].parse().unwrap_or(0.0),
            double_notches: values[17].parse().unwrap_or(0),
            color: values[18].parse().unwrap_or(-1),
            markes: values[19].parse().unwrap_or(0),
            width: values[55].parse().unwrap_or(0.0),
        };

        let dates = [values[191].parse().unwrap_or(0), values[192].parse().unwrap_or(0), values[193].parse().unwrap_or(0)];
        let date = Date {
            created: values[186].parse().unwrap_or(0),
            modified: values[187].parse().unwrap_or(0),
            pending: values[188].parse().unwrap_or(0),
            effective: values[189].parse().unwrap_or(0),
            finished: values[190].parse().unwrap_or(0),
            dates,
        };

        let alloc = Alloc {
            group: values[230].to_string(),
            sequence: values[231].parse().unwrap_or(0),
            target_min: values[232].parse().unwrap_or(0.0),
            target_max: values[233].parse().unwrap_or(0.0),
            section_splits: values[256].parse().unwrap_or(0),
            priority: values[257].parse().unwrap_or(0),
        };

        let statuses = [values[183].to_string(), values[184].to_string(), values[185].to_string()];
        let checksums = [values[258].to_string(), values[259].to_string(), values[260].to_string(), values[261].to_string(), values[262].to_string()];

        const EMPTY_STRING:String = String::new();

        let mut planogram = Planogram {
            name,
            key: values[2].to_string(),
            width: values[3].parse().unwrap_or(0.0),
            height: values[4].parse().unwrap_or(0.0),
            depth: values[5].parse().unwrap_or(0.0),
            color: values[6].parse().unwrap_or(0),
            back_depth: values[7].parse().unwrap_or(0.0),
            draw_back: values[8].parse().unwrap_or(1),
            base_width: values[9].parse().unwrap_or(0.0),
            base_height: values[10].parse().unwrap_or(0.0),
            base_depth: values[11].parse().unwrap_or(0.0),
            draw_base: values[12].parse().unwrap_or(0),
            base_color: values[13].parse().unwrap_or(0),
            notch,
            draw_pegs: values[20].parse().unwrap_or(0),
            draw_peg_holes: values[21].parse().unwrap_or(0),
            traffic_flow: values[22].parse().unwrap_or(0),
            auto_created: values[23].parse().unwrap_or(0),
            shape_id: values[24].to_string(),
            bit_map_id: values[25].to_string(),
            combined_perforamnce_index: values[53].parse().unwrap_or(0.0),
            number_of_stores: values[54].parse().unwrap_or(0.0),
            values: [0.0; 50],
            descriptions: [EMPTY_STRING; 50],
            flags: [0; 10],
            fill_pattern: values[166].parse().unwrap_or(0),
            segments_to_print: values[167].to_string(),
            file_name: values[168].to_string(),
            changed: values[169].parse().unwrap_or(0),
            layout_file_name: values[170].to_string(),
            notes: values[171].to_string(),
            date,
            statuses,
            source_file_type: values[182].parse().unwrap_or(2),
            created_by: values[194].to_string(),
            modified_by: values[195].to_string(),
            floor_bitmap_id: values[196].to_string(),
            door_transparency: values[197].parse().unwrap_or(0.5),
            floor_tile_width: values[198].parse().unwrap_or(12.0),
            floor_tile_depth: values[199].parse().unwrap_or(12.0),
            case_multiplier: values[206].parse().unwrap_or(1.5),
            days_supply: values[207].parse().unwrap_or(1.5),
            demand_cycle_length: values[208].parse().unwrap_or(1),
            peak_saftey_factor: values[209].parse().unwrap_or(1.0),
            backroom_stock: values[210].parse().unwrap_or(0.0),
            demands: [0.0; 7],
            delivery_schedule: values[218].to_string(),
            id: values[219].to_string(),
            department: values[220].to_string(),
            part_id: values[221].to_string(),
            gln: values[222].to_string(),
            custom_data: values[223].to_string(),
            abbrevated_name: values[226].to_string(),
            category: values[227].to_string(),
            subcategory: values[228].to_string(),
            source: values[229].parse().unwrap_or(0),
            alloc,
            can_segment: values[234].parse().unwrap_or(0),
            can_split: values[235].parse().unwrap_or(-1),
            pg_status: values[236].parse().unwrap_or(-1),
            pg_core_percent: values[237].parse().unwrap_or(0),
            pg_core_note: values[238].to_string(),
            pg_warnings_count: values[239].parse().unwrap_or(0),
            pg_errors_count: values[240].parse().unwrap_or(0),
            pg_action_list: values[241].to_string(),
            pg_max_stage_reduce: values[242].parse().unwrap_or(0.0),
            pg_max_stage_fillout: values[243].parse().unwrap_or(0.0),
            pg_type: values[244].parse().unwrap_or(0),
            pg_server_name: values[253].to_string(),
            model_file_name: values[245].to_string(),
            pr_status: values[254].parse().unwrap_or(-1),
            use_as_subplanogram: values[263].parse().unwrap_or(0),
            movement_peroid: values[255].parse().unwrap_or(0),
            checksums,
            merch,
            inventory,
            number_of_fixtures: 0,
            number_of_segments: 0,
            number_of_drawings: 0,
            number_of_recommendations: 0,
            warning: EMPTY_STRING,
            warnings_count: 0,
            warning_number: 0,
        };

        let mut i = 0;
        for num in 106..=155 {
            planogram.values[i] = values[num].parse().unwrap_or(0.0);
            i += 1;
        }

        let mut i = 0;
        for num in 56..=105 {
            planogram.descriptions[i] = values[num].to_string();
            i += 1;
        }

        let mut i = 0;
        for num in 156..=165 {
            planogram.flags[i] = values[num].parse().unwrap_or(0);
            i += 1;
        }

        let mut i = 0;
        for num in 211..=217 {
            planogram.demands[i] = values[num].parse().unwrap_or(0.0);
            i += 1;
        }

        return planogram;
    }
}
