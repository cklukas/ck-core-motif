#!/usr/bin/env python3
"""Check NumberBox public state APIs reject null widget handles."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "NumberBox.c").read_text(encoding="utf-8")
assert source.count("widget == NULL || !XmIsNumberBox(widget)") >= 3
assert source.count("widget != NULL && XmIsNumberBox(widget)") >= 3
print("PASS NumberBox state APIs validate null widgets")
