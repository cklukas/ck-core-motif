#!/usr/bin/env python3
"""Check XmString unparse conversion totals are not narrowed unsafely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("static void\nunparse_text")
body = source[start:source.index("/*\n * unparse_is_plausible", start)]
assert "size_t converted" in body
assert "converted == (size_t)-1" in body
assert "(size_t)INT_MAX - (size_t)*length" in body
assert "*length += len * sizeof(wchar_t)" not in body
print("PASS XmString unparse length accumulation is bounded")
