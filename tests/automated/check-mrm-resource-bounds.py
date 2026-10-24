#!/usr/bin/env python3
"""Check Mrm resource-descriptor index sizing."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "size_t index_len = strlen(index);",
    "index_len > SIZE_MAX - sizeof(RGMResourceDesc)",
    "sizeof(RGMResourceDesc) + index_len > USHRT_MAX",
    "descsiz = sizeof(RGMResourceDesc) + index_len",
):
    if needle not in text:
        raise SystemExit(f"Mrm resource descriptor guard missing: {needle}")

print("Mrm resource descriptor sizing is bounded")
