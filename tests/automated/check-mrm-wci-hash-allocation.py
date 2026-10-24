#!/usr/bin/env python3
"""Check Mrm widget-class hash insertion validates allocation inputs."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwci.c"
text = source.read_text(encoding="utf-8")
start = text.rindex("hash_insert_name (")
end = text.index("\n}\n", start)
region = text[start:end]
for needle in (
    "if (htable == NULL || c_text == NULL)",
    "name_length > INT_MAX",
    "name_length > SIZE_MAX - sizeof(URMHashTableEntry) - 1",
    "if (az_new_name == NULL)",
):
    if needle not in region:
        raise SystemExit(f"Mrm hash insertion guard is missing: {needle}")

print("Mrm widget-class hash insertion validates inputs and allocation")
