#!/usr/bin/env python3
"""Check XmStringExtent validates output pointers before dereferencing."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("void \nXmStringExtent(")
end = source.index("\n}\n", start)
function = source[start:end]

assert "if (width != NULL)" in function
assert "if (height != NULL)" in function
assert "width == NULL || height == NULL" in function
assert function.index("width == NULL || height == NULL") < function.index("_XmStrOptimized(string)")

print("XmStringExtent guards null output pointers")
