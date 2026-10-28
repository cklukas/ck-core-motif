#!/usr/bin/env python3
"""Check clipboard length results reject invalid or overflowing records."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("XmClipboardInquireLength(")
end = source.index("XmClipboardInquirePendingItems(", start)
body = source[start:end]

assert "if (matchformat->itemLength < 0)" in body
assert "ULONG_MAX / CONVERT_32_FACTOR" in body
assert "loc_length = (unsigned long)matchformat->itemLength *" in body
assert "loc_length = matchformat->itemLength;" not in body

print("Clipboard length results reject invalid records")
