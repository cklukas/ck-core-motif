#!/usr/bin/env python3
"""Check optimized XmString concatenation handles allocation failure."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
marker = "_XmStrCreate(opt_str, XmSTRING_OPTIMIZED, a_len + b_len);"
start = source.index(marker)
window = source[start:start + 300]

assert "if (opt_str == NULL)" in window
assert "XmStringFree(a);" in window
assert "XmStringFree(b);" in window
assert "return NULL;" in window

print("optimized XmString concatenation handles allocation failure")
