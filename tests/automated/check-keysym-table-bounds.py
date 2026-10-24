#!/usr/bin/env python3
"""Check bounded sizing for the KeySym-table resource converter."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text(encoding="utf-8")
start = text.index("CvtStringToKeySymTable(", 10000)
end = text.index("\n/*ARGSUSED*/\nstatic void", start)
region = text[start:end]

for needle in (
    "size_t in_str_size = 0;",
    "size_t i, comma_count;",
    "if (in_str_size == SIZE_MAX)",
    "comma_count > (SIZE_MAX / sizeof(KeySym)) - 2",
):
    if needle not in region:
        raise SystemExit(f"KeySym-table capacity guard missing: {needle}")

print("KeySym-table allocation sizing is bounded")
