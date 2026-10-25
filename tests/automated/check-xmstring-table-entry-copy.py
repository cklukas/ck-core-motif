#!/usr/bin/env python3
"""Check XmString table concatenation propagates entry-copy failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("XmStringTableToXmString(")
end = source.index("\nXmString\nXmStringPutRendition", start)
region = source[start:end]

assert "copied = XmStringCopy(table[i]);" in region
assert "if (copied == NULL)" in region
assert "XmStringFree(tmp1);" in region
assert region.index("if (copied == NULL)") < region.index("XmStringConcatAndFree(tmp1, copied)")
print("PASS XmStringTableToXmString propagates entry-copy failure")
