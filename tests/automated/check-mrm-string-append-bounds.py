#!/usr/bin/env python3
"""Check ordinary string append sizing before Mrm record allocation."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.rindex("UrmCWR__AppendString (")
end = text.index("\n}\n", start)
region = text[start:end]
for needle in (
    "size_t",
    "len = strlen (stg)",
    "len == SIZE_MAX",
    "len >= MrmMaxResourceSize",
    "len++",
):
    if needle not in region:
        raise SystemExit(f"Mrm string append guard missing: {needle}")

print("Mrm string append sizing is bounded")
