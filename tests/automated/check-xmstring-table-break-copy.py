#!/usr/bin/env python3
"""Check XmString table concatenation propagates break-copy failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("XmStringTableToXmString(")
end = source.index("\nXmString\nXmStringPutRendition", start)
region = source[start:end]

assert "copied_break;" in region
assert "copied_break = break_comp ? XmStringCopy(break_comp) : NULL;" in region
assert "if (break_comp != NULL && copied_break == NULL)" in region
assert "XmStringFree(tmp2);" in region
print("PASS XmStringTableToXmString propagates break-copy failure")
