#!/usr/bin/env python3
"""Check wide-string append sizing before Mrm record allocation."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmCWR__AppendWcharString (")
end = text.index("\n}\n", start)
region = text[start:end]
for needle in (
    "size_t",
    "cnt;",
    "cnt == SIZE_MAX",
    "MrmMaxResourceSize / sizeof(wchar_t)",
    "len = (cnt+1) * sizeof(wchar_t)",
):
    if needle not in region:
        raise SystemExit(f"Mrm wide-string append guard missing: {needle}")

print("Mrm wide-string append sizing is bounded")
