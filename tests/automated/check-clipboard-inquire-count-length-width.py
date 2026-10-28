#!/usr/bin/env python3
"""Check clipboard format-name lengths retain strlen's size width."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("int \nXmClipboardInquireCount(")
end = source.index("int \nXmClipboardInquireFormat(", start)
body = source[start:end]

assert "size_t temp;" in body
assert "temp = strlen(str);" in body
assert "if ( temp > loc_maxlength)" in body

print("Clipboard inquiry preserves atom-name length width")
