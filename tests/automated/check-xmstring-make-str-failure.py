#!/usr/bin/env python3
"""Check MakeStr detects failed segment-copy insertion."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("MakeStrFromSeg(XmStringContext")
end = source.index("\n\f", start)
region = source[start:end]

assert region.count("_XmStrContError(start) = TRUE;") >= 2
assert "_XmEntrySegmentCount(_XmStrEntry(str)[0]) == 0" in region
assert "_XmStrEntryCount(str) == 0" in region
print("PASS MakeStr rejects failed segment-copy insertion")
