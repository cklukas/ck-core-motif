#!/usr/bin/env python3
"""Check XmString table parsing rejects null source elements."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("XmStringTableParseStringArray(")
end = source.index("\n\nXtPointer *\nXmStringTableUnparse", start)
region = source[start:end]

assert "if (strings[i] == NULL)" in region
assert "XmStringFree(strs[--i]);" in region
assert region.index("if (strings[i] == NULL)") < region.index("XmStringParseText")
print("PASS XmStringTableParseStringArray rejects null source elements")
