#!/usr/bin/env python3
"""Check XmClipboardCopyByName bounds serialized length accumulation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("XmClipboardCopyByName(")
end = source.index("XmClipboardUndoCopy(", start)
body = source[start:end]

assert "unsigned long item_length_increment;" in body
assert "formatheader->itemLength < 0" in body
assert "(unsigned long)LONG_MAX" in body
assert "formatheader->itemLength += (long)item_length_increment;" in body

print("Clipboard copy-by-name length accumulation is bounded")
