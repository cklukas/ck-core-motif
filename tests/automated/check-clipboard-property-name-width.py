#!/usr/bin/env python3
"""Check clipboard property-name comparisons retain strlen width."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static int \nGetWindowProperty(")
end = source.index("static Boolean\nClipboardHeaderRecordValid(", start)
body = source[start:end]

assert "size_t i, len;" in body
assert "len = strlen(match);" in body
assert "for(i = 0; i < len; i++)" in body

print("Clipboard property-name comparison preserves width")
