#!/usr/bin/env python3
"""Ensure extended segment separator scans stay within the segment."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c"
text = source.read_text(encoding="utf-8")
start = text.index("processExtendedSegments(\n")
end = text.index("\n\t    charset_copy = XtMalloc", start)
block = text[start:end]

assert "while (len < seglen && esptr[len] != STX)" in block
assert "if (len >= seglen)" in block
assert "separator is missing" in block
assert "while (esptr[len] != STX)" not in block

print("ResEncod extended-segment bounds: ok")
