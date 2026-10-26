#!/usr/bin/env python3
"""Check Flyout public visibility APIs reject null widget handles."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Flyout.c").read_text(encoding="utf-8")
assert source.count("widget == NULL || !XmIsFlyout(widget)") >= 2
assert "widget != NULL && XmIsFlyout(widget)" in source
print("PASS Flyout visibility APIs validate null widgets")
