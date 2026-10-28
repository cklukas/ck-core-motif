#!/usr/bin/env python3
"""Check validated clipboard target counts are narrowed only after division."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("XmClipboardInquireCount(")
end = source.index("XmClipboardInquireFormat(", start)
body = source[start:end]

assert "loc_count_len / sizeof(Atom) > (unsigned long)INT_MAX" in body
assert "loc_count = (int)(loc_count_len / sizeof(Atom));" in body
assert "loc_count = (int) loc_count_len / sizeof(Atom);" not in body

print("Clipboard target-count narrowing is validated")
