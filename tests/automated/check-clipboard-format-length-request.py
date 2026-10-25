#!/usr/bin/env python3
"""Check that format-length metadata reads are bounded to one unit."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text(encoding="utf-8")
start = source.index("static int \nClipboardGetLenFromFormat(")
end = source.index("static Boolean\nClipboardHeaderRecordValid(", start)
region = source[start:end]

assert "1, /*length: one 32-bit format-length value*/" in region
assert "10000000" not in region
assert "format != 32" in region
print("PASS clipboard format-length property reads are bounded")
