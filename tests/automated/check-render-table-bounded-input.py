#!/usr/bin/env python3
"""Check render-table import parses only its supplied property length."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
start = source.index("XmRenderTableCvtFromProp(")
end = source.index("\n}\n", start)
body = source[start:end]

assert "prop_copy = (char *)XtMalloc((Cardinal)len + 1);" in body
assert "memcpy(prop_copy, prop, len);" in body
assert "prop_copy[len] = '\\0';" in body
assert "ReadToken(prop_copy" in body
assert "XtFree(prop_copy);" in body

print("render-table import is bounded by its property length")
