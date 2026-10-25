#!/usr/bin/env python3
"""Check MakeStr propagates segment-slice failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("MakeStr(XmStringContext")
end = source.index("\n\f", start)
region = source[start:end]

assert "segment = MakeStrFromSeg(start);" in region
assert "if (segment == NULL)" in region
assert "XmStringFree(str);" in region
print("PASS MakeStr propagates segment-slice failure")
