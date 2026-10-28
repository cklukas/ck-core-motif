#!/usr/bin/env python3
"""Check clipboard length inquiries abort on every lock failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("int \nXmClipboardInquireLength(")
end = source.index("int \nXmClipboardInquirePendingItems(", start)
body = source[start:end]

assert "if ( status != ClipboardSuccess )" in body
assert "return status;" in body

print("Clipboard length inquiry aborts on lock failure")
