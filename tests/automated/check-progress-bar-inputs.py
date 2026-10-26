#!/usr/bin/env python3
"""Check ProgressBar public state APIs reject null widget handles."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ProgressBar.c").read_text(encoding="utf-8")
assert source.count("widget == NULL || !XmIsProgressBar(widget)") >= 4
assert "return widget != NULL && XmIsProgressBar(widget)" in source
print("PASS ProgressBar state APIs validate null widgets")
