#!/usr/bin/env python3
"""Check that the UTF-8-to-UCS2 helper bounds allocation and input reads."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xm.c").read_text()
start = source.index("_XmUtf8ToUcs2(")
end = source.index("\n\n\n/***************************************/", start)
region = source[start:end]
assert "seg_len > SIZE_MAX / sizeof(XChar2b)" in region
assert "size_t remaining;" in region
assert "remaining >= 2" in region
assert "remaining >= 3" in region
assert "draw_text[1] & 0xC0" in region
assert "draw_text[2] & 0xC0" in region
assert "char *ep;" not in region
print("UTF-8 conversion bounds allocation and malformed reads")
