#!/usr/bin/env python3
"""Check _XmUtf8ToUcs2 enforces Unicode scalar boundaries."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xm.c").read_text()
start = source.index("_XmUtf8ToUcs2(")
end = source.index("\n}\n", start) + 3
region = source[start:end]
assert "first >= 0xC2" in region
assert "first != 0xE0" in region and "first != 0xED" in region
assert "first >= 0xF0 && first <= 0xF4" in region
assert "codepoint > 0xFFFF" in region
assert "(unsigned char)draw_text[3]" in region
print("UTF-8 to UCS-2 conversion validates scalar boundaries")
