#!/usr/bin/env python3
"""Validate the Media node manifest against the pinned PSBBV Core contract."""

import json
from pathlib import Path

from jsonschema import Draft202012Validator

ROOT = Path(__file__).resolve().parents[1]
SCHEMA_PATH = ROOT / "schemas" / "psbbv-node-manifest.schema.json"
MANIFEST_PATH = ROOT / "psbbv-node.json"


def load_json(path: Path):
    with path.open("r", encoding="utf-8") as stream:
        return json.load(stream)


def main() -> int:
    schema = load_json(SCHEMA_PATH)
    manifest = load_json(MANIFEST_PATH)
    Draft202012Validator.check_schema(schema)
    validator = Draft202012Validator(schema)
    errors = sorted(validator.iter_errors(manifest), key=lambda error: list(map(str, error.absolute_path)))
    if errors:
        for error in errors:
            location = ".".join(str(part) for part in error.absolute_path) or "<root>"
            print(f"INVALID {location}: {error.message}")
        return 1
    print("PSBBV Media manifest: valid against pinned Core schema.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
