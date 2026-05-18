# File Format And Parser Flow

## Record model

- PSA is parsed as plain text, one record per line.
- Fields are comma-delimited after preprocessing (see below).
- Column `0` is the record indicator for all typed records.
- Lines whose column `0` does not match a supported indicator are ignored.
- Supported indicators (case-insensitive):
  - `Project`
  - `Planogram`
  - `Fixture`
  - `Product`
  - `Position`
  - `Performance`
  - `Segment`
  - `Drawing`
  - `Divider`

## Encoding and line endings

- Lines are typically terminated with CRLF (`\r\n`). LF (`\n`) alone is also acceptable; parsers should normalize line endings before processing.
- The character encoding is typically an extended ASCII superset (for example, Windows-1252). Parsers may treat the input as an opaque byte stream for field splitting.
- Preamble or comment lines beginning with a semicolon (`;`) may appear anywhere in the file and should be treated as ordinary non-matching lines.

## Line preprocessing

Before splitting by comma, each input line is transformed in this exact order:

1. **Unescape doubled backslashes:** Replace every occurrence of two consecutive backslash characters with a single backslash character. This handles the case where a literal backslash needs to be represented inside a field.
2. **Escape commas:** Replace every occurrence of a backslash character immediately followed by a comma character with Unicode character U+201A (single low-9 quotation mark).
3. **Split on commas:** Split the line on every comma character that was not replaced in step 2.

Notes:

- There is no CSV quote-aware parsing.
- Escaped commas are not restored back to literal commas later; they stay as Unicode `U+201A` in parsed strings.
- A line without commas is treated as a single-field row.
- Other backslash sequences (for example, a backslash followed by a double-quote, carriage return, or line feed) are not transformed and remain as literal characters in the parsed field.

## Header/version capture

- Line `0` (the very first line of the file) is always captured as the file header text (rejoined with commas).
- Line `1` (the second line) is always captured as the file version text.

These two lines still go through normal record handling after capture. Any additional preamble or comment lines — for example, lines beginning with a semicolon — are treated as ordinary non-matching lines and ignored.

## Dispatch and parser state

- The parser keeps a current planogram context key (int), initialized to `0`.
- On `Planogram`:
  - Parse the record.
  - Parse the planogram key field as an integer.
  - If key parsing fails, use key value `"0"` and set the current planogram context key to `0`.
  - Update the current planogram context key from the parsed planogram key.
- On `Fixture`, `Position`, and `Segment`:
  - Parse record.
  - Attach the current planogram context key as contextual association to the record.

If a `Fixture`, `Position`, or `Segment` appears before any valid `Planogram`, its associated contextual key is `0`.

## Field handling rules

- Leading/trailing whitespace in string fields is preserved.
- Numeric parsing accepts the raw field text and applies the numeric conversion rules below.
- Columns above the highest mapped index for a record type are ignored.
- Files may contain additional empty trailing fields beyond the highest mapped index for a record type. These extra fields must be ignored and must not cause a record to be rejected.

## Optional section boundary operators

The presence checks for optional tail sections use different comparison operators depending on the record type (for example, `>= 214` for Project versus `> 229` for Planogram). Implementers must match the exact operator and threshold documented for each record type. Using the wrong operator will cause a parser to either miss optional data or incorrectly reject valid records.

## Numeric conversion rules

All numeric field parsing follows these rules:

- Integer fields: if input is null/empty/unparseable, use the field's documented default (or `0` if none is specified).
- Floating-point fields: if input is null/empty/unparseable, use the field's documented default (or `0.0` if none is specified).
- Floating-point fields: on successful parse, round to two decimal places with midpoint values rounded away from zero.
- Floating-point source values may contain many decimal places of precision. The rounding rule must be applied to the successfully parsed value before storing or comparing.
- Floating-point parsing is locale-sensitive (runtime default numeric format).
- 64-bit integer fields: if input is null/empty/unparseable, use the field's documented default (or `0` if none is specified).

## Error handling behavior

- Most record types require required columns to exist; short lines can fail parsing.
- Invalid `Product` rows are skipped and parsing continues.
- Invalid rows for other record types terminate parsing.
