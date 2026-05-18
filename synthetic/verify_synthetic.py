#!/usr/bin/env python3

import json
import subprocess
import sys
from pathlib import Path


ROOT_DIR = Path(__file__).resolve().parent.parent
SYNTHETIC_DIR = ROOT_DIR / "synthetic"
OUT_DIR = SYNTHETIC_DIR / "out"
MANIFEST_PATH = OUT_DIR / "manifest.json"
CLI_PATH = ROOT_DIR / "build" / "psa-cli"

RECORD_MAP = {
    "Project": "projects",
    "Planogram": "planograms",
    "Fixture": "fixtures",
    "Product": "products",
    "Position": "positions",
    "Performance": "performances",
    "Segment": "segments",
    "Drawing": "drawings",
    "Divider": "dividers",
}


def parse_document(path: Path) -> dict:
    result = subprocess.run(
        [str(CLI_PATH), str(path)],
        check=False,
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        raise RuntimeError(f"parser failed for {path.name}: {result.stderr.strip()}")
    try:
        return json.loads(result.stdout)
    except json.JSONDecodeError as exc:
        raise RuntimeError(f"invalid JSON output for {path.name}: {exc}") from exc


def main() -> int:
    if not CLI_PATH.exists():
        print(f"missing CLI binary: {CLI_PATH}", file=sys.stderr)
        return 1
    if not MANIFEST_PATH.exists():
        print(f"missing manifest: {MANIFEST_PATH}", file=sys.stderr)
        return 1

    manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
    failures = []

    for item in manifest:
        file_name = item["file"]
        psa_path = OUT_DIR / file_name
        if not psa_path.exists():
            failures.append(f"missing synthetic file: {file_name}")
            continue

        doc = parse_document(psa_path)
        expected = item["counts"]

        for rec_name, json_key in RECORD_MAP.items():
            actual_count = len(doc.get(json_key, []))
            expected_count = expected.get(rec_name, 0)
            if actual_count != expected_count:
                failures.append(
                    f"{file_name}: {rec_name} expected {expected_count}, got {actual_count}"
                )

        print(f"ok {file_name}")

    if failures:
        print("synthetic verification failed:", file=sys.stderr)
        for line in failures:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("all synthetic files matched manifest counts")
    return 0


if __name__ == "__main__":
    sys.exit(main())
