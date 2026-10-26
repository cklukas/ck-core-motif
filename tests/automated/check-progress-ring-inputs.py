#!/usr/bin/env python3
"""Check ProgressRing public state APIs reject null widget handles."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ProgressRing.c").read_text(encoding="utf-8")
assert source.count("widget == NULL || !XmIsProgressRing(widget)") >= 4
assert "return widget != NULL && XmIsProgressRing(widget)" in source
print("PASS ProgressRing state APIs validate null widgets")
