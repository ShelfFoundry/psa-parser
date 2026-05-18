# Product Record (`Product`)

## Shape

- Indicator: column `0` = `Product`.
- Parsed columns: `1..273`.

## Column mapping

Identity and physical attributes:

- `1`: UPC text
- `2`: internal/business id text
- `3`: product name text
- `4`: product key text
- `5..7`: width/height/depth (double)
- `8`: color code (int)
- `9`: abbreviated name text
- `10`: size value (double)
- `11`: unit-of-measure text
- `12`: manufacturer text
- `13`: category text
- `14`: supplier text
- `15`: inner-pack quantity (int)
- `16..18`: nesting factors on X/Y/Z (double)
- `19`: peg-hole count (int, default `1`)
- `20..28`: peg-hole geometry values (double)
- `29`: packaging style code (int)
- `30`: peg profile/id text
- `31`: finger-space Y (double)
- `32`: jumble factor (double)
- `33`: price (double)
- `34`: case cost (double)
- `35`: tax code (int, default `1`)
- `36`: unit movement (double)
- `37`: share (double)
- `38`: case multiple (double)
- `39`: days supply (double)
- `40`: combined performance index (double)
- `41`: peg span (int)
- `42`: minimum units (int)
- `43`: maximum units (int)
- `44`: shape reference id (string)
- `45`: bitmap/image override id (string)

Alternative pack-size blocks:

- `46..53`: tray dimensions/counts
- `54..61`: case dimensions/counts
- `62..69`: display dimensions/counts
- `70..77`: alternate dimensions/counts
- `78..85`: loose dimensions/counts

Merchandising and extension blocks:

- `86..112`: X/Y/Z merchandising rules (int)
- `113`: number of positions (int, default `1`)
- `114..163`: text extension slots 1-50 (string)
- `164..213`: numeric extension slots 1-50 (double)
- `214..223`: flag extension slots 1-10 (int)

Squeeze/model/status tail:

- `224..229`: minimum/maximum squeeze factors on X/Y/Z (double, default `1`)
- `230`: fill pattern code (int)
- `231`: model file name/path text (string)
- `232`: brand text (string)
- `233`: subcategory text (string)
- `234`: weight (double)
- `235`: planogram alias text (string)
- `236`: changed marker/status (int)
- `237`: front overhang (double)
- `238`: finger-space X (double)
- `239..248`: external database keys 1-10 (64-bit int)
- `249`: status text (string)
- `250..257`: date/timestamp numeric slots (int)
- `258`: created-by text (string)
- `259`: modified-by text (string)
- `260`: transparency (double)
- `261`: peak safety factor (double)
- `262`: backroom stock value (double)
- `263`: delivery schedule text (string)
- `264`: part id text (string)
- `265`: authority level code (int)
- `266`: bitmap/image unit override code (int)
- `267`: model-file lookup mode code (int)
- `268`: default merchandising-style code (int)
- `269`: automatic-model flag/code (int)
- `270`: custom payload text (string)
- `271`: database GUID text (string)
- `272`: source code (int)
- `273`: technical key (64-bit int)

## Row-failure behavior

- Invalid `Product` rows are skipped while parsing continues.
