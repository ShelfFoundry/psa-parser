# Fixture Record (`Fixture`)

## Shape

- Indicator: column `0` = `Fixture`.
- Base section parses `1..157`.
- Optional section: if column count is `> 158`, parse `158..159`.

## Column mapping

Core placement and dimensions:

- `1`: fixture type code (int)
- `2`: fixture name (string)
- `3`: fixture key text (string)
- `4..9`: x/width/y/height/z/depth (double)
- `10..12`: slope/angle/roll (double)
- `13`: color code (int)
- `14`: assembly text (string)
- `15..23`: spacing/start/wall/curve/merch factors (double)
- `24`: collision-check-other-fixtures flag (int, default `1`)
- `25`: collision-check-other-positions flag (int, default `1`)
- `26`: can-obstruct flag (int)
- `27..32`: overhang values (double)
- `33`: default merchandising-style code (int, default `-1`)
- `34..36`: divider dimensions (double)
- `37`: combinable flag (int)
- `38..44`: grille/notch/peg spacing parameters (double)
- `45`: primary label-format name (string)
- `46`: secondary label-format name (string)
- `47`: shape reference id (string)
- `48`: bitmap/image reference id (string)

Merchandising rules and extension fields:

- `49..57`: X-axis merchandising rules (int)
- `58..66`: Y-axis merchandising rules (int)
- `67..75`: Z-axis merchandising rules (int)
- `76..105`: text extension slots 1-30 (string)
- `106..135`: numeric extension slots 1-30 (double)
- `136..145`: flag extension slots 1-10 (int)

Metadata tail:

- `146`: location id/code (int, default `-1`)
- `147`: fill pattern code (int)
- `148`: model file name/path text (string)
- `149`: weight capacity (double)
- `150`: changed marker/status (int)
- `151..153`: divider placement controls (int)
- `154`: transparency (double)
- `155`: hide-when-printing flag (int)
- `156`: product association text (string)
- `157`: part id text (string)
- `158` (optional): hide-view-dimensions flag (int)
- `159` (optional): GLN/location code text (string)
