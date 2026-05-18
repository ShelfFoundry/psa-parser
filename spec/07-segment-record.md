# Segment Record (`Segment`)

## Shape

- Indicator: column `0` = `Segment`.
- Base section parses `1..11`.
- Extended section parses `12..51` when column count is `> 12`.

Compatibility note:

- Extended-section presence check is looser than its full read range; partially present extended data can fail parsing.

## Column mapping

Base segment geometry:

- `1`: segment name (string)
- `2`: segment key text (string)
- `3..8`: x/width/y/height/z/depth (double)
- `9`: angle (double)
- `10`: X offset (double)
- `11`: Y offset (double)

Extended section:

- `12`: door flag/code (int)
- `13`: door direction code (int)
- `14..23`: text extension slots 1-10 (string)
- `24..33`: numeric extension slots 1-10 (double)
- `34..43`: flag extension slots 1-10 (int)
- `44`: frame width (double)
- `45`: frame height (double)
- `46`: changed marker/status (int)
- `47`: frame color code (int, default `-1`)
- `48`: frame fill pattern code (int)
- `49`: part id text (string)
- `50`: GLN/location code text (string)
- `51`: custom payload text (string)
