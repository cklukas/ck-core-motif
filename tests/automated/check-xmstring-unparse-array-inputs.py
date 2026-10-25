#!/usr/bin/env python3
"""Check XmString table unparsing rejects null source elements."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("XmStringTableUnparse(")
end = source.index("\n\nXmString\nXmStringTableToXmString", start)
region = source[start:end]

assert "if (table[i] == NULL)" in region
assert "XtFree((char *)strs[--i]);" in region
assert region.index("if (table[i] == NULL)") < region.index("XmStringUnparse")
print("PASS XmStringTableUnparse rejects null source elements")
