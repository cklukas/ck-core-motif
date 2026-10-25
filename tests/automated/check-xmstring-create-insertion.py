#!/usr/bin/env python3
"""Check that non-optimized XmString creation validates segment insertion."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("_XmStringNCreate(char *text,")
end = source.index("\n}\n", start)
function = source[start:end]

assert "_XmStringSegmentNew(str, 0, (_XmStringEntry)&seg, True);" in function
assert "if (_XmStrEntryCount(str) == 0)" in function
assert "_XmStrFree(str);" in function
assert "return((XmString) NULL);" in function

print("non-optimized XmString creation validates segment insertion")
