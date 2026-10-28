#!/usr/bin/env python3
"""Check XmClipboardEndCopy validates item IDs and offset arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("XmClipboardEndCopy(")
end = source.index("XmClipboardEndRetrieve(", start)
body = source[start:end]

assert "unsigned long newitemoffset;" in body
assert "if (itemid <= 0)" in body
assert "header->currItems > ULONG_MAX / sizeof(itemId)" in body
assert "ULONG_MAX - header->dataItemList" in body
assert "newitemoffset > ULONG_MAX / CONVERT_32_FACTOR" in body

print("Clipboard end-copy offset arithmetic is bounded")
