#!/usr/bin/env python3
"""Check XmStringTableToXmString rejects null table elements."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("XmStringTableToXmString(")
end = source.index("\nXmString\nXmStringPutRendition", start)
region = source[start:end]

assert "if (table[i] == NULL)" in region
assert "XmStringFree(tmp1);" in region
assert region.index("if (table[i] == NULL)") < region.index("XmStringCopy(table[i])")
print("PASS XmStringTableToXmString rejects null table elements")
