#!/usr/bin/env python3
"""Check pending-item traversal uses unsigned-width counters."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("int \nXmClipboardInquirePendingItems(")
end = source.index("int\nXmClipboardRegisterFormat(", start)
body = source[start:end]

assert "unsigned long loc_count, i;" in body
assert "for ( i = 0; i < header->currItems; i++ )" in body

print("Clipboard pending-item counters preserve unsigned width")
