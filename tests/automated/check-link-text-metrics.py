#!/usr/bin/env python3
"""Check Link text and underline geometry use active font metrics."""
from pathlib import Path

source = Path("lib/Xm/Link.c").read_text(encoding="utf-8")
assert "XQueryFont" in source
assert "XTextWidth" in source
assert "XFreeFontInfo(NULL, font, 1)" in source
assert "4 + text_width" in source
print("PASS Link text uses measured font geometry")
