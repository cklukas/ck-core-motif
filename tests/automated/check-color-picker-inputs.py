#!/usr/bin/env python3
"""Check ColorPicker public value APIs reject null widget handles."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorPicker.c").read_text(encoding="utf-8")
assert "widget != NULL && XmIsColorPicker(widget)" in source
assert source.count("widget == NULL || !XmIsColorPicker(widget)") >= 2
print("PASS ColorPicker value APIs validate null widgets")
