pub struct Merch {
    pub x: MerchDetails,
    pub y: MerchDetails,
    pub z: MerchDetails,
}

pub struct MerchDetails {
    pub min: i32,
    pub max: i32,
    pub uprights: i32,
    pub caps: i32,
    pub placement: i32,
    pub number: i32,
    pub size: i32,
    pub direction: i32,
    pub squeeze: i32,
}
