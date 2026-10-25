#!/usr/bin/env python3
"""Check that XmStringGenerate validates rendition cache failures."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("XmString\nXmStringGenerate(")
end = source.index("\n}\n", start)
function = source[start:end]

assert function.count("_XmStringIndexGetTag(rend_index) == NULL") >= 3
assert "if (cached_rend == NULL)" in function
assert function.count("XmStringFree(result);") >= 4

print("XmStringGenerate validates rendition cache failures")
