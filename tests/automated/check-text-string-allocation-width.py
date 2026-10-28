#!/usr/bin/env python3
"""Check Text string buffers use validated Xt and byte-count widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
start = source.index("_XmStringSourceGetString(")
end = source.index("static void", start)
body = source[start:end]

assert "XtMalloc((Cardinal)allocation)" in body
assert "(size_t)block.length" in body
assert "XtMalloc(allocation)" not in body

print("Text string allocation widths are explicit")
