#!/usr/bin/env python3
"""Check TreeView public model/state APIs reject null widget handles."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TreeView.c").read_text(encoding="utf-8")
start = source.index("void XmTreeViewSetNodes")
region = source[start:]
assert "widget == NULL || !XmIsTreeView(widget)" in region
assert "widget != NULL && XmIsTreeView(widget)" in region
assert "widget == NULL || !XmIsTreeView(widget) || count" in region
print("PASS TreeView model/state APIs validate null widgets")
