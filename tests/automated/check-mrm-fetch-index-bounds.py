#!/usr/bin/env python3
"""Check Mrm fetch-set-values index descriptor sizing."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwvalues.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmFetchSetValues (")
end = text.index("\n}\n", start)
region = text[start:end]
for needle in (
    "sizeof(RGMResourceDesc) > MrmMaxResourceSize",
    "indexlen > MrmMaxResourceSize - sizeof(RGMResourceDesc)",
    "indexlen > SIZE_MAX - sizeof(RGMResourceDesc)",
):
    if needle not in region:
        raise SystemExit(f"Mrm fetch-index bound is missing: {needle}")

print("Mrm fetch-set-values index sizing is bounded")
