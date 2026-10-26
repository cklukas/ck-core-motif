#!/usr/bin/env python3
"""Check SearchBox public value APIs reject null widget handles."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SearchBox.c").read_text(encoding="utf-8")
assert "widget != NULL && XmIsSearchBox(widget)" in source
assert source.count("widget == NULL || !XmIsSearchBox(widget)") >= 2
print("PASS SearchBox value APIs validate null widgets")
