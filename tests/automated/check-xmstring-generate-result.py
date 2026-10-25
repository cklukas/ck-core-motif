#!/usr/bin/env python3
"""Check that XmStringGenerate propagates parse failure before rendition use."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("XmString\nXmStringGenerate(")
end = source.index("\n}\n", start)
function = source[start:end]

assert "result = XmStringParseText" in function
assert "if (result == NULL)" in function
assert function.index("if (result == NULL)") < function.index("_XmStrOptimized(result)")

print("XmStringGenerate propagates parse failure")
