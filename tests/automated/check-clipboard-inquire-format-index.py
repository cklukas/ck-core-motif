#!/usr/bin/env python3
"""Check clipboard format inquiries reject non-positive 1-based indexes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("int \nXmClipboardInquireFormat(")
end = source.index("int \nXmClipboardInquireLength(", start)
body = source[start:end]

assert "if (n <= 0 || outlength == NULL" in body
assert "return ClipboardBadFormat;" in body

print("Clipboard format inquiries reject non-positive indexes")
