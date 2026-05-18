# Project Record (`Project`)

## Shape

- Indicator: column `0` = `Project` (case-insensitive).
- Base section requires columns `0..212`.
- Optional section:
  - if column count is `>= 214`: parse column `213` as delivery schedule text
  - if column count is `>= 215`: parse column `214` as custom payload text
  - if column count is `>= 216`: parse column `215` as family/group key number

## Column mapping

Core identity and planning settings:

- `1`: project display name (string, default `"Space Planning Project"` when empty)
- `2`: project key text (string)
- `3`: primary numeric key (int)
- `4`: layout file name/path text (string)
- `5`: movement period (int, default `7`)
- `6`: case-multiple factor (double, default `1.5`)
- `7`: days-supply factor (double, default `1.5`)
- `8`: demand cycle length (int, default `1`)
- `9`: peak safety factor (double, default `1.0`)
- `10`: backroom stock factor/value (double, default `0.0`)
- `11`: peg profile/id text (string)
- `12`: measurement mode code (int, default `0`)
- `13`: number of stores (int, default `0`)

Merchandising rule groups:

- `14..22`: X-axis merchandising rules (min/max/uprights/endcaps/placement/number/size/direction/squeeze) (int)
- `23..31`: Y-axis merchandising rules (min/max/uprights/endcaps/placement/number/size/direction/squeeze) (int)
- `32..40`: Z-axis merchandising rules (min/max/uprights/endcaps/placement/number/size/direction/squeeze) (int)

Demand and generic extension slots:

- `41..68`: demand slots 1-28 (double, default `0.0`)
- `69..74`: inventory-model option codes (int, default `0`)
- `75..124`: numeric extension slots 1-50 (double, default `0.0`)
- `125..174`: text extension slots 1-50 (string)
- `175..184`: flag/boolean-like extension slots 1-10 (int, default `0`)

Metadata block:

- `185`: notes text (string)
- `186`: changed marker/status (int, default `0`)
- `187..196`: external database keys 1-10 (64-bit int, default `-1`)
- `197..200`: performance-override mode codes (int, default `2`)
- `201`: status text (string)
- `202..209`: date/timestamp numeric slots (int, default `0`)
- `210`: created-by text (string)
- `211`: modified-by text (string)
- `212`: planogram-specific inventory mode (int, default `0`)
- `213` (optional): delivery schedule text (string)
- `214` (optional): custom payload text (string)
- `215` (optional): family/group key (int, default `-1`)
