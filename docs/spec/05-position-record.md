# Position Record (`Position`)

## Shape

- Indicator: column `0` = `Position`.
- Parsed columns: `1..166`.

## Column mapping

Identity and geometry:

- `1`: UPC text
- `2`: internal/business id text
- `3`: position key text
- `4..12`: x/width/y/height/z/depth/slope/angle/roll (double)
- `13`: merchandising style code (int)
- `14..16`: horizontal/vertical/depth facings (int, default `1`)

Capacity/orientation controls:

- `17..20`: X-capacity controls (int; orientation default `2`)
- `21..24`: Y-capacity controls (int; orientation default `4`)
- `25..28`: Z-capacity controls (int; reversed default `1`, orientation default `2`)
- `29`: orientation code (int)
- `30..32`: jumble dimensions (double)
- `33..35`: merchandising-style dimensions (double)
- `36..38`: full dimensions (double)
- `39..41`: subunit counts on X/Y/Z (int)
- `42`: peg profile/id text (string)
- `43`: manual-units flag (int, default `1`)
- `44..46`: rank/order on X/Y/Z (int, default `1`)
- `47`: peg span (int)
- `48`: always-float flag (int)
- `49`: primary label-format name (string)
- `50`: secondary label-format name (string)

Merchandising and extension blocks:

- `51..77`: X/Y/Z merchandising rules (int)
- `78..107`: text extension slots 1-30 (string)
- `108..137`: numeric extension slots 1-30 (double)
- `138..147`: flag extension slots 1-10 (int)

Target-space and metadata tail:

- `148..150`: target-space usage flags on X/Y/Z (int)
- `151..153`: target-space values on X/Y/Z (double)
- `154`: location id/code (int, default `-1`)
- `155`: changed marker/status (int)
- `156`: replenishment minimum (int)
- `157`: replenishment maximum (int)
- `158`: shape reference id (string)
- `159`: bitmap/image override id (string)
- `160`: hide-when-printing flag (int)
- `161`: part id text (string)
- `162`: bitmap/image unit override code (int)
- `163`: automatic-model flag/code (int, default `-1`)
- `164`: custom payload text (string)
- `165`: X-capacity includes-units flag (int)
- `166`: Y-capacity includes-units flag (int)
