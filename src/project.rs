use crate::{date::Date, inventory::Inventory, merch::{Merch, MerchDetails}};

pub struct Project {
    pub name: String,
    pub key: String,
    pub primary_key: i32,
    pub layout_file: String,
    pub movement_period: i32,
    pub case_mult: f32,
    pub days_supply: f32,
    pub demand_cycle_length: i32,
    pub peak_safety_factor: f32,
    pub backroom_stock: f32,
    pub peg_id: String,
    pub measurement: i32,
    pub number_of_stores: i32,
    pub merch: Merch,
    pub demands: [f32;28],
    pub inventory: Inventory,
    pub values: [f32;50],
    pub descriptions: [String;50],
    pub flags: [i32;10],
    pub notes: String,
    pub changed: i32,
    pub use_performance_price: i32,
    pub use_performance_cost: i32,
    pub use_performance_tax_code: i32,
    pub use_performance_movement: i32,
    pub status: String,
    pub date: Date,
    pub created_by: String,
    pub modified_by: String,
    pub planogram_specific_inventory: i32,
    pub delivery_schedule: String,
    pub custom_data: String,
    pub number_of_planograms: i32,
    pub number_of_products: i32,
    pub number_of_suppliers: i32,
    pub sales: f32,
    pub cost: f32,
    pub margin: f32,
    pub capacity_cost: f32,
    pub capacity_retail: f32,
    pub annual_profit: f32,
    pub roii_cost: f32,
    pub roii_retail: f32,
    pub movement: f32,
    pub capacity: i32,
    pub number_of_products_allocated: i32,
    pub sales_allocated: f32,
    pub cost_allocated: f32,
    pub movement_allocated: f32,
    pub margin_allocated: f32,
    pub annual_profit_allocated: f32,
    pub warnings: i32,
    pub roii_cost_allocated: f32,
    pub roii_retail_allocated: f32,
    pub profit: f32,
    pub profit_allocated: f32,
    pub capacity_unrestricted: i32,
}

impl Project {
    pub fn new(line: String) -> Self {
        let values:Vec<&str> = line.split(',').into_iter().collect();

        let mut name = String::from("Planogram Project");
        if values[1] != "" {
            name = values[1].to_string();
        }

        let merch = Merch {
            x: MerchDetails {
                min: values[14].parse().unwrap_or(0),
                max: values[15].parse().unwrap_or(0),
                uprights: values[16].parse().unwrap_or(0),
                caps: values[17].parse().unwrap_or(0),
                placement: values[18].parse().unwrap_or(3),
                number: values[19].parse().unwrap_or(1),
                size: values[20].parse().unwrap_or(1),
                direction: values[21].parse().unwrap_or(0),
                squeeze: values[22].parse().unwrap_or(0),
            },
            y: MerchDetails {
                min: values[23].parse().unwrap_or(-1),
                max: values[24].parse().unwrap_or(-1),
                uprights: values[25].parse().unwrap_or(-1),
                caps: values[26].parse().unwrap_or(-1),
                placement: values[27].parse().unwrap_or(2),
                number: values[28].parse().unwrap_or(3),
                size: values[29].parse().unwrap_or(1),
                direction: values[30].parse().unwrap_or(-1),
                squeeze: values[31].parse().unwrap_or(-1),
            },
            z: MerchDetails {
                min: values[32].parse().unwrap_or(-1),
                max: values[32].parse().unwrap_or(-1),
                uprights: values[34].parse().unwrap_or(-1),
                caps: values[35].parse().unwrap_or(-1),
                placement: values[36].parse().unwrap_or(3),
                number: values[37].parse().unwrap_or(3),
                size: values[38].parse().unwrap_or(1),
                direction: values[39].parse().unwrap_or(-1),
                squeeze: values[40].parse().unwrap_or(-1),
            },
        };

        let inventory = Inventory {
            manual: values[69].parse().unwrap_or(0),
            case_multiplier: values[70].parse().unwrap_or(0),
            days_supply: values[71].parse().unwrap_or(0),
            peak: values[72].parse().unwrap_or(0),
            min_units: values[73].parse().unwrap_or(0),
            max_units: values[74].parse().unwrap_or(0),
        };

        let dates = [values[207].parse().unwrap_or(0), values[208].parse().unwrap_or(0), values[209].parse().unwrap_or(0)];

        let date = Date {
            created: values[202].parse().unwrap_or(0),
            modified: values[203].parse().unwrap_or(0),
            pending: values[204].parse().unwrap_or(0),
            effective: values[205].parse().unwrap_or(0),
            finished: values[206].parse().unwrap_or(0),
            dates,
        };

        const EMPTY_STRING:String = String::new();

        let mut project = Project {
            name,
            key: values[2].to_string(),
            primary_key: values[3].parse().unwrap_or(0),
            layout_file: values[4].to_string(),
            movement_period: values[5].parse().unwrap_or(7),
            case_mult: values[6].parse().unwrap_or(1.5),
            days_supply: values[7].parse().unwrap_or(1.5),
            demand_cycle_length: values[8].parse().unwrap_or(1),
            peak_safety_factor: values[9].parse().unwrap_or(1.0),
            backroom_stock: values[10].parse().unwrap_or(0.0),
            peg_id: values[11].to_string(),
            measurement: values[12].parse().unwrap_or(0),
            number_of_stores: values[13].parse().unwrap_or(0),
            merch,
            inventory,
            demands: [0.0; 28],
            values: [0.0; 50],
            descriptions: [EMPTY_STRING; 50],
            flags: [0; 10],
            notes: values[185].to_string(),
            changed: values[186].parse().unwrap_or(0),
            use_performance_price: values[197].parse().unwrap_or(2),
            use_performance_cost: values[198].parse().unwrap_or(2),
            use_performance_tax_code: values[199].parse().unwrap_or(2),
            use_performance_movement: values[200].parse().unwrap_or(2),
            status: values[201].to_string(),
            date,
            created_by: values[210].to_string(),
            modified_by: values[211].to_string(),
            planogram_specific_inventory: values[212].parse().unwrap_or(0),
            delivery_schedule: values[213].to_string(),
            custom_data: values[214].to_string(),
            annual_profit: 0.0,
            annual_profit_allocated: 0.0,
            capacity: 0,
            capacity_cost: 0.0,
            capacity_retail: 0.0,
            capacity_unrestricted: 0,
            cost: 0.0,
            cost_allocated: 0.0,
            margin: 0.0,
            margin_allocated: 0.0,
            movement: 0.0,
            movement_allocated: 0.0,
            number_of_planograms: 0,
            number_of_products: 0,
            number_of_suppliers: 0,
            number_of_products_allocated: 0,
            profit: 0.0,
            profit_allocated: 0.0,
            roii_cost: 0.0,
            roii_cost_allocated: 0.0,
            roii_retail: 0.0,
            roii_retail_allocated: 0.0,
            sales: 0.0,
            sales_allocated: 0.0,
            warnings: 0,
        };

        let mut i = 0;
        for num in 41..=68 {
            project.demands[i] = values[num].parse().unwrap_or(0.0);
            i += 1;
        }

        let mut i = 0;
        for num in 75..=124 {
            project.values[i] = values[num].parse().unwrap_or(0.0);
            i += 1;
        }

        let mut i = 0;
        for num in 125..=174 {
            project.descriptions[i] = values[num].to_string();
            i += 1;
        }

        let mut i = 0;
        for num in 175..=184 {
            project.flags[i] = values[num].parse().unwrap_or(0);
            i += 1;
        }

        return project;
    }
}
