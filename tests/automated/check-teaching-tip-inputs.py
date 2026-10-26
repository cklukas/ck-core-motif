#!/usr/bin/env python3
"""Check TeachingTip visibility APIs reject null widget handles."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TeachingTip.c").read_text(encoding="utf-8")
assert source.count("widget == NULL || !XmIsTeachingTip(widget)") >= 2
assert "widget != NULL && XmIsTeachingTip(widget)" in source
print("PASS TeachingTip visibility APIs validate null widgets")
