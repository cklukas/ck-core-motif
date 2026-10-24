#!/usr/bin/env python3
"""Check Mrm pointer-list string append sizing."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmptrlist.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmPlistAppendString (")
end = text.index("\n}\n", start)
region = text[start:end]
for needle in (
    "stg_len = strlen(stg);",
    "stg_len == SIZE_MAX",
    "XtMalloc (stg_len + 1)",
    "memcpy (newstg, stg, stg_len + 1)",
):
    if needle not in region:
        raise SystemExit(f"Mrm pointer-list string bound is missing: {needle}")

print("Mrm pointer-list string allocation is bounded")
