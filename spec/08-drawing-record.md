# Drawing Record (`Drawing`)

## Shape

- Indicator: column `0` = `Drawing`.
- Parsed columns: `1..47`.

## Column mapping

- `1`: drawing type code (int)
- `2`: drawing name (string)
- `3`: drawing key text (string)
- `4..9`: x/width/y/height/z/depth (double)
- `10`: foreground color code (int, default `-1`)
- `11`: background-fill flag/code (int)
- `12`: background color code (int, default `16777215`)
- `13`: created-in-view code (int, default `2`)
- `14`: show-in-all-views flag (int)
- `15`: word-wrap flag (int)
- `16`: circular flag (int)
- `17..19`: start point x/y/z (double)
- `20..22`: end point x/y/z (double)
- `23`: drawing text content (string)
- `24`: text scale code (int, default `2`)
- `25`: outline flag (int, default `1`)
- `26`: callout flag/code (int)
- `27..31`: font metrics/weight numeric values (64-bit int; weight default `400`)
- `32..39`: font style and rendering codes (int; pitch/family default `32`)
- `40`: font face name (string, default `"Arial"` when empty)
- `41..43`: callout anchor x/y/z (double)
- `44`: center-text flag (int, default `1`)
- `45`: changed marker/status (int)
- `46`: hide-when-printing flag (int)
- `47`: custom payload text (string)
