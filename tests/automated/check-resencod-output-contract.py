#!/usr/bin/env python3
"""Check that the compound-text resource converter initializes its output."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c").read_text()
start = source.index("static Boolean \ncvtTextToXmString(")
end = source.index("\n}\n", start)
body = source[start:end]

assert "to->addr = NULL;" in body
assert "to->size = 0;" in body
assert body.index("to->addr = NULL;") < body.index("ctx = (ct_context *)XtMalloc")

print("compound-text resource conversion initializes output")
