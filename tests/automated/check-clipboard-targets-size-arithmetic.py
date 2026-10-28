#!/usr/bin/env python3
"""Check TARGETS conversion uses checked byte and element arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static Boolean\nClipboardConvertProc(")
end = source.index("static int \nClipboardRequestDataAndWait(", start)
body = source[start:end]

assert "size_t atom_bytes;" in body
assert "atom_bytes = (size_t)(count + 2) * sizeof(*ptr);" in body
assert "XtMalloc((Cardinal)atom_bytes)" in body
assert "*size = (unsigned long)n + 2UL;" in body

print("Clipboard TARGETS size arithmetic is explicit and bounded")
