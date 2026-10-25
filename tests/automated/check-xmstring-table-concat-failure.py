#!/usr/bin/env python3
"""Check XmString table concatenation propagates intermediate failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("XmStringTableToXmString(")
end = source.index("\nXmString\nXmStringPutRendition", start)
region = source[start:end]

assert "copied = XmStringCopy(table[i]);" in region
assert "tmp2 = XmStringConcatAndFree(tmp1, copied);" in region
assert "if (tmp2 == NULL)" in region
assert region.index("if (tmp2 == NULL)") < region.index("str = XmStringConcatAndFree")
print("PASS XmStringTableToXmString propagates intermediate concat failure")
