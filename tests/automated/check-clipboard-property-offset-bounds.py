#!/usr/bin/env python3
"""Check XGetWindowProperty offsets are bounded before signed conversion."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static int \nGetWindowProperty(")
end = source.index("static Boolean\nClipboardHeaderRecordValid(", start)
body = source[start:end]

assert "if (offset > (unsigned long)LONG_MAX)" in body
assert "(long)offset, /*offset*/" in body

print("Clipboard property offsets are bounded before Xlib conversion")
