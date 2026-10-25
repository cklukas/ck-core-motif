#!/usr/bin/env python3
"""Check that XmString text concatenation guards output growth."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("_XmStringGetTextConcat(")
end = source.index("\n/****************************************************************\n * Allocates a copy", start)
region = source[start:end]

assert "SIZE_MAX - OutLen - 1" in region
assert "(Cardinal)~0" in region
assert "char *new_out = XtRealloc" in region
assert "if (!new_out)" in region
assert region.index("if (!new_out)") < region.index("memcpy")
assert "XtFree(OutStr);" in region
print("PASS XmString text concatenation growth is guarded")
