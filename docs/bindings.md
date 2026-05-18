# Bindings Guide (Language Agnostic)

This guide explains how to integrate `psa-parser` from any language with FFI support.

## Integration Options

There are two common approaches:

- Direct library integration (recommended for performance and control)
- CLI integration (recommended for quick prototypes)

## Option 1: Direct Library Integration

Use the C API from `include/psa.h`.

Primary entry points:

- `psa_parse_file(...)`: streaming callback API
- `psa_parse_file_to_json_document(...)`: single JSON document API

Supporting helpers:

- `psa_record_to_json(...)`
- `psa_file_meta_to_json(...)`

### Streaming vs Document API

- Use `psa_parse_file(...)` when you want to process records incrementally and keep memory usage low.
- Use `psa_parse_file_to_json_document(...)` when you want one top-level object with arrays for each record type.

## Option 2: CLI Integration

- Default CLI output is one top-level JSON document.
- `--stream` emits NDJSON.
- `--summary` emits counts to `stderr`.

CLI can be useful when your runtime cannot use C FFI directly.

## Data Ownership and Lifetime

For `psa_parse_file(...)` callbacks:

- String pointers inside `psa_record_t` are parser-owned.
- They are valid only during the callback invocation.
- Copy strings in the callback if you need to retain them.

Do not store parser-owned pointers after callback return.

## Record Model Notes

- Records are tagged via `psa_record_t.type` and a C union.
- Access only the union member matching `type`.
- Many fields are fixed-size arrays (extension slots), often containing empty-string/default values.

This is intentional for stable field positions and deterministic FFI layouts.

## Error Handling

Core return codes:

- `PSA_OK` (0)
- `PSA_ERR_IO` (-1)
- `PSA_ERR_PARSE` (-2)
- `PSA_ERR_ABORT` (-3)

Provide an `errbuf` to receive parse/I/O details.

## Numeric and Text Behavior

- Numeric parsing applies documented defaults when input is empty/invalid.
- Doubles are rounded to 2 decimals (midpoint away from zero).
- Preprocessing preserves most backslash sequences except specified PSA escapes.

If your application wants display-specific normalization (for example, converting `\"` sequences), do that in your application layer.

## Recommended Binding Pattern

1. Map C enums and structs from `psa.h`.
2. Wrap return codes in native errors/exceptions.
3. For streaming API, copy strings during callback.
4. Convert fixed arrays into native slices/lists as needed.
5. Add a high-level API in your language that returns domain-friendly models.

## Compatibility Advice

- Keep your binding layer thin and faithful to C behavior.
- Perform optional transformations in a separate higher-level layer.
- Pin against a specific library version/commit in production.
