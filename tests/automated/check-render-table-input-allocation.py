#!/usr/bin/env python3
"""Check render-table parsing guards its mutable input copy."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text()
start = text.index("cvtStringToXmRenderTable(")
end = text.index("static Boolean\nCvtStringToRenderTable", start)
block = text[start:end]

if "s = XtNewString((char *)from->addr);" not in block:
    raise SystemExit("render-table parser does not copy its input")
if "if (s == NULL)" not in block:
    raise SystemExit("render-table input copy has no null guard")

print("render-table parsing handles input-copy allocation failure")
