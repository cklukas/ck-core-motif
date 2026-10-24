#!/usr/bin/env python3
"""Check Mrm hash lookup and deletion validate table/name inputs."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwci.c"
text = source.read_text(encoding="utf-8")
for name in ("hash_find_name", "hash_delete_name"):
    start = text.rindex(name + " (")
    end = text.index("\n}\n", start)
    region = text[start:end]
    if "htable == NULL || c_text == NULL" not in region:
        raise SystemExit(f"{name} null-input guard is missing")
    if "name_length > INT_MAX" not in region:
        raise SystemExit(f"{name} length narrowing guard is missing")

print("Mrm hash lookup and deletion validate inputs")
