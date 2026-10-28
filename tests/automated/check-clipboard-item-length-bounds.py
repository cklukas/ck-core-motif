#!/usr/bin/env python3
"""Check clipboard item-length accumulation is signed-range safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("int \nXmClipboardCopy(")
end = source.index("int \nXmClipboardCopyByName(", start)
body = source[start:end]

assert "unsigned long item_length_increment;" in body
assert "(unsigned long)LONG_MAX - item_length_increment" in body
assert "formatptr->itemLength += (long)item_length_increment;" in body

print("Clipboard item-length accumulation is bounded")
