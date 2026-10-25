#!/usr/bin/env python3
"""Check DatePicker text uses active font metrics."""
from pathlib import Path

source = Path("lib/Xm/DatePicker.c").read_text(encoding="utf-8")
assert "XQueryFont" in source
assert "XFreeFontInfo(NULL, font, 1)" in source
assert "text_baseline(picker)" in source
print("PASS DatePicker text uses measured font baseline")
