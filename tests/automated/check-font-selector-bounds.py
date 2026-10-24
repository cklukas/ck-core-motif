#!/usr/bin/env python3
"""Check that FontSelector XLFD fallback handling is length-safe."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text(encoding="utf-8")
start = text.index("static void\nDisplayCurrentFont")
end = text.index("\n}\n", start) + 3
region = text[start:end]
assert "strcpy(right_buf, ptr)" not in region
assert "strcpy(left_buf, font)" not in region
assert "sprintf(fbuf" not in region
assert "snprintf(fbuf" in region
assert "XtMalloc((Cardinal)i + 1)" in region
assert "snprintf(buf, sizeof(buf)" in region
print("FontSelector XLFD fallback formatting is bounded")
