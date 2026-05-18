# psa-parser

`psa-parser` is an independent C library and CLI for reading PSA planogram files.

This project is not affiliated with, endorsed by, or sponsored by Blue Yonder, JDA, or any related vendor. Product and company names are trademarks of their respective owners.

The project does not include proprietary documentation, vendor software, or customer planogram files.

The implementation is based on independent analysis of PSA files and operational experience with planogram workflows.

## Requirements

- Linux/macOS shell environment
- `gcc`
- `make`

No external runtime dependencies are required for the parser library or CLI.

## Project Layout

- `include/psa.h`: public API
- `src/`: parser, record mapping, JSON helpers
- `cli/main.c`: command-line interface
- `tests/`: unit and synthetic integration tests
- `docs/spec/`: file format and record mapping specification

## Development Commands

Build library, CLI, and tests:

```bash
make all
```

Run full test suite:

```bash
make test
```

Clean build artifacts:

```bash
make clean
```

## Running the CLI

Build first:

```bash
make all
```

Parse a PSA file and emit one top-level JSON document (default):

```bash
./build/psa-cli path/to/file.psa
```

Emit JSON Lines stream instead (one JSON object per line):

```bash
./build/psa-cli --stream path/to/file.psa
```

Show parse summary instead of JSON output:

```bash
./build/psa-cli --summary path/to/file.psa
```

Emit one top-level JSON document explicitly (same as default):

```bash
./build/psa-cli --document path/to/file.psa
```

Help:

```bash
./build/psa-cli --help
```

## Library API

- Main entry point: `psa_parse_file(...)`
- Limits-aware parser: `psa_parse_file_ex(...)`
- In-memory parsing: `psa_parse_buffer(...)`, `psa_parse_buffer_ex(...)`
- Callback receives typed records via `psa_record_t`
- JSON helpers:
  - `psa_record_to_json(...)` (returns `PSA_OK`/`PSA_ERR_*`, reports `out_written`/`out_needed`)
  - `psa_file_meta_to_json(...)` (returns `PSA_OK`/`PSA_ERR_*`, reports `out_written`/`out_needed`)
  - `psa_parse_file_to_json_document(...)` (returns `PSA_OK`/`PSA_ERR_*`, reports `out_written`/`out_needed`)
  - `psa_parse_buffer_to_json_document(...)` (same contract for in-memory PSA content)

See `include/psa.h` for full API details and record structures.

## Compatibility

`psa-parser` is not a complete or official PSA implementation. It supports the subset of PSA records and file structures described in this repository.

Known and unknown record handling should be treated as part of the compatibility surface.

This parser is tested against synthetic fixtures and a limited set of independently analyzed PSA files. Compatibility with all vendor versions is not guaranteed.

## Versioning

- Current release target: `v0.1.0`.
- API/ABI compatibility for `0.x` releases is best effort and may evolve based on implementation feedback.
- Error code semantics (`PSA_OK`, `PSA_ERR_*`) are intended to remain stable across minor releases.
- Production consumers should pin to a specific tag or commit.
