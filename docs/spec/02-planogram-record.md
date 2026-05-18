# Planogram Record (`Planogram`)

## Shape

- Indicator: column `0` = `Planogram`.
- Base section requires columns `0..228`.
- Optional section A: if column count is `> 229`, parse `229..232`.
- Optional section B: if column count is `> 233`, parse `233..254`.

Compatibility note:

- Section A/B presence checks are looser than the full read range; partially present tails can fail parsing.

## Column mapping

Identity and fixture-envelope geometry:

- `1`: planogram name (string)
- `2`: planogram key text (string)
- `3..5`: width/height/depth (double)
- `6`: display color code (int)
- `7`: back depth (double)
- `8`: draw-back flag (int, default `1`)
- `9..11`: base width/height/depth (double)
- `12`: draw-base flag (int, default `1`)
- `13`: base color code (int)
- `14..21`: notch/peg drawing controls (int/double; notch color default `-1`)
- `22`: traffic-flow code (int)
- `23`: auto-created marker (int)
- `24`: shape reference id (string)
- `25`: bitmap/image reference id (string)

Axis merchandising rules:

- `26..34`: X-axis merchandising rules (int)
- `35..43`: Y-axis merchandising rules (int)
- `44..52`: Z-axis merchandising rules (int)

Custom and extension blocks:

- `53`: combined performance index (double)
- `54`: store count (int)
- `55`: notch width (double)
- `56..105`: text extension slots 1-50 (string)
- `106..155`: numeric extension slots 1-50 (double)
- `156..165`: flag extension slots 1-10 (int)

File/meta/inventory block:

- `166`: fill pattern code (int)
- `167`: printable segment-selection text (string)
- `168`: source file name text (string)
- `169`: changed marker/status (int)
- `170`: layout file name/path text (string)
- `171`: notes text (string)
- `172..181`: external database keys 1-10 (64-bit int, default `-1`)
- `182`: source-file type code (int, default `2`)
- `183..185`: status text slots 1-3 (string)
- `186..193`: date/timestamp numeric slots (int)
- `194`: created-by text (string)
- `195`: modified-by text (string)
- `196`: floor bitmap/image reference id (string)
- `197`: door transparency (double, default `0.5`)
- `198`: floor tile width (double, default `12`)
- `199`: floor tile depth (double, default `12`)
- `200..205`: inventory-model option codes (int)
- `206`: case-multiple factor (double, default `1.5`)
- `207`: days-supply factor (double, default `1.5`)
- `208`: demand cycle length (int, default `1`)
- `209`: peak safety factor (double, default `1`)
- `210`: backroom stock value (double)
- `211..217`: demand slots 1-7 (double)
- `218`: delivery schedule text (string)
- `219`: external/business id text (string)
- `220`: department text (string)
- `221`: part id text (string)
- `222`: GLN/location code text (string)
- `223`: custom payload text (string)
- `224`: planogram GUID text (string)
- `225`: database GUID text (string)
- `226`: abbreviated name text (string)
- `227`: category text (string)
- `228`: subcategory text (string)

Optional section A (`229..232`):

- source code (int), allocation group text (string), allocation sequence (int), minimum allocation target (double)

Optional section B (`233..254`):

- allocation max target, split/segment/status/score controls, warning/error counts, action text,
  stage limits, type/model/family/version/parent references, processing timestamp code,
  server text, final status code (mixed int/double/string; defaults where present are `-1`)
