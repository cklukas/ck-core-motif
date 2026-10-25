#!/usr/bin/env python3
"""Check ColorPicker value text uses active font metrics."""
from pathlib import Path

source = Path("lib/Xm/ColorPicker.c").read_text(encoding="utf-8")
assert "XQueryFont" in source
assert "XFreeFontInfo(NULL, font, 1)" in source
assert "baseline = text_baseline(picker)" in source
print("PASS ColorPicker text uses measured font baseline")
