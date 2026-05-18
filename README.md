# psa-parser

`psa-parser` is an independent C library and CLI for reading PSA planogram files.

This project is not affiliated with, endorsed by, or sponsored by Blue Yonder, JDA, or any related vendor. Product and company names are trademarks of their respective owners.

The project does not include proprietary documentation, vendor software, or customer planogram files.

The implementation is based on independent analysis and operational experience with PSA files.

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
- `spec/`: file format and record mapping specification

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

Parse a PSA file and emit JSON Lines (one JSON object per record):

```bash
./build/psa-cli path/to/file.psa
```

Show parse summary instead of record JSON:

```bash
./build/psa-cli --summary path/to/file.psa
```

Emit one top-level JSON document with metadata and record arrays:

```bash
./build/psa-cli --document path/to/file.psa
```

Help:

```bash
./build/psa-cli --help
```

## Library API

- Main entry point: `psa_parse_file(...)`
- Callback receives typed records via `psa_record_t`
- JSON helpers:
  - `psa_record_to_json(...)`
  - `psa_file_meta_to_json(...)`
  - `psa_parse_file_to_json_document(...)`

See `include/psa.h` for full API details and record structures.
