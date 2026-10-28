#!/usr/bin/env python3
"""Check pending-item allocation narrows only after its Cardinal bound."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("XmClipboardInquirePendingItems(")
body = source[start:]

assert "header->currItems > (unsigned long)UINT_MAX /" in body
assert "XtMalloc( (Cardinal)" in body
assert "XtMalloc( (size_t)" not in body

print("Clipboard pending-item allocation is Cardinal-bounded")
