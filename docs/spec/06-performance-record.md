# Performance Record (`Performance`)

## Shape

- Indicator: column `0` = `Performance`.
- Parsed columns: `1..149`.

## Column mapping

Identity and base performance metrics:

- `1`: UPC text
- `2`: internal/business id text
- `3`: performance key text
- `4`: price (double)
- `5`: case cost (double)
- `6`: tax code (int, default `1`)
- `7`: unit movement (double)
- `8`: share (double)
- `9`: combined performance index (double)

Extension blocks (first half):

- `10..19`: text extension slots 1-10 (string)
- `20..29`: numeric extension slots 1-10 (double)
- `30..39`: flag extension slots 1-10 (int)
- `40`: changed marker/status (int)
- `41..60`: text extension slots 11-30 (string)
- `61..80`: numeric extension slots 11-30 (double)

Inventory/assortment controls:

- `81`: case multiple (double)
- `82`: days supply (double)
- `83`: peak safety factor (double, default `-0.01`)
- `84`: backroom stock value (double, default `-0.01`)
- `85`: minimum units (int, default `-1`)
- `86`: maximum units (int, default `-1`)
- `87`: delivery schedule text (string)
- `88`: replenishment minimum (int)
- `89`: replenishment maximum (int)
- `90`: assortment rank (int)
- `91`: recommended facings (int)
- `92..95`: assortment strategy/tactic/reason/action text
- `96`: part id text
- `97`: cluster/group name text
- `98`: target distribution store count (int)
- `99`: target distribution percent (double)
- `100`: assortment note text

Extension blocks (second half) and recommendation fields:

- `101..120`: text extension slots 31-50 (string)
- `121..140`: numeric extension slots 31-50 (double)
- `141`: custom payload text (string)
- `142`: recommended orientation code (int, default `-1`)
- `143`: recommended merchandising-style code (int, default `-1`)
- `144`: ignore-recommendations flag (int)
- `145`: priority code (int)
- `146`: priority description text (string)
- `147`: force-list flag/code (int)
- `148`: planogram reason text (string)
- `149`: max stage-reduction value (double)
