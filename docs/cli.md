# CLI Guide

This project ships a CLI at `./build/psa-cli`.

## Build

```bash
make all
```

## Usage

```bash
./build/psa-cli [--summary|--stream] <file.psa>
```

By default (no mode flag), the CLI emits a single top-level JSON document.

## Output Modes

- Default: one JSON document containing metadata and per-record arrays
- `--stream`: JSON Lines (NDJSON): metadata line first, then one line per record
- `--summary`: parse summary to `stderr` (no JSON records to `stdout`)

## Examples

Default document output:

```bash
./build/psa-cli ./example.psa
```

Streaming output (NDJSON):

```bash
./build/psa-cli --stream ./example.psa
```

Summary output:

```bash
./build/psa-cli --summary ./example.psa
```

Help:

```bash
./build/psa-cli --help
```

## Default Document Shape

The default output object includes:

- `header`
- `version`
- `projects`
- `planograms`
- `fixtures`
- `products`
- `positions`
- `performances`
- `segments`
- `drawings`
- `dividers`

Each record key is always present and is always an array.

## Exit Behavior

- Exit code `0` on success
- Exit code `1` on parse/file/usage errors
- Parse errors are written to `stderr`
