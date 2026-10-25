#!/usr/bin/env python3
"""Check XmString table conversion propagates break concatenation failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("XmStringTableToXmString(")
end = source.index("\nXmString\nXmStringPutRendition", start)
region = source[start:end]

assert "str = XmStringConcatAndFree(tmp2, copied_break);" in region
assert "if (str == NULL)" in region
assert region.index("if (str == NULL)") < region.index("tmp1 = str;")
print("PASS XmStringTableToXmString propagates break concat failure")
