#!/usr/bin/env python3
"""Ensure clipboard payload byte conversion cannot wrap."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeClipboardSource(")
end = source.index("/* XmeDragSource", start)
body = source[start:end]

assert "unsigned long unit = format2 == 8 ? 1UL" in body
assert "size2 > ULONG_MAX / unit" in body
assert "XtFree((char *)value);" in body
assert "size2 = BYTELENGTH(size2, format2);" in body

print("PASS clipboard payload multiplication is overflow-checked")
