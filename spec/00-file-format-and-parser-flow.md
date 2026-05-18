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

## Line preprocessing

Before splitting by comma, each input line is transformed in this exact order:

1. Remove all `\\` sequences.
2. Replace all `\,` sequences with Unicode `U+201A`.
3. Split on literal commas.

Notes:

- There is no CSV quote-aware parsing.
- Escaped commas are not restored back to literal commas later; they stay as Unicode `U+201A` in parsed strings.
- A line without commas is treated as a single-field row.

## Header/version capture

- Line `0` (first line) is captured as the file header text (rejoined with commas).
- Line `1` (second line) is captured as the file version text.

These two lines still go through normal record handling after capture. Any additional metadata/comment-style preamble lines are treated as ordinary non-matching lines and ignored.

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

## Numeric conversion rules

All numeric field parsing follows these rules:

- Integer fields: if input is null/empty/unparseable, use the field's documented default (or `0` if none is specified).
- Floating-point fields: if input is null/empty/unparseable, use the field's documented default (or `0.0` if none is specified).
- Floating-point fields: on successful parse, round to two decimal places with midpoint values rounded away from zero.
- Floating-point parsing is locale-sensitive (runtime default numeric format).
- 64-bit integer fields: if input is null/empty/unparseable, use the field's documented default (or `0` if none is specified).

## Error handling behavior

- Most record types require required columns to exist; short lines can fail parsing.
- Invalid `Product` rows are skipped and parsing continues.
- Invalid rows for other record types terminate parsing.
