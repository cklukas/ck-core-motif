#!/usr/bin/env python3
"""Check size_t accounting for Mrm literal index strings."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwvalues.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "size_t\t\tindexlen = 0",
    "size_t literal_len = strlen((char*)args[ndx].value)",
    "indexlen > SIZE_MAX - sizeof(RGMResourceDesc)",
):
    if needle not in text:
        raise SystemExit(f"Mrm literal-length guard missing: {needle}")

print("Mrm literal index sizing is bounded")
