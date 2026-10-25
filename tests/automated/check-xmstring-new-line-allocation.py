#!/usr/bin/env python3
"""Check that XmString substring line creation handles allocation failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "static Boolean new_line(_XmString string)",
    "if (line == NULL)",
    "if (entries == NULL)",
    "if (!new_line(str))",
):
    if needle not in text:
        raise SystemExit(f"XmString line allocation guard is missing: {needle}")

print("XmString line allocation guards present")
