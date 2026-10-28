#!/usr/bin/env python3
"""Check clipboard format indexes use the validated unsigned comparison."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("XmClipboardInquireFormat(")
end = source.index("XmClipboardInquireLength(", start)
body = source[start:end]

assert "if (n <= 0" in body
assert "loc_matchlength >= (unsigned long)n" in body
assert "loc_matchlength >= n" not in body

print("Clipboard format-index comparison is signedness-safe")
