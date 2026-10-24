#!/usr/bin/env python3
"""Check Mrm's shared string allocator validates terminator sizing."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmvm.c"
text = source.read_text(encoding="utf-8")
start = text.index("Urm__UT_AllocString (")
end = text.index("\n}\n", start)
region = text[start:end]
for needle in (
    "stg_len = strlen(stg);",
    "stg_len == SIZE_MAX",
    "XtMalloc (stg_len + 1)",
    "memcpy (new_stg, stg, stg_len + 1)",
):
    if needle not in region:
        raise SystemExit(f"Mrm string-allocation bound is missing: {needle}")

print("Mrm shared string allocation is bounded")
