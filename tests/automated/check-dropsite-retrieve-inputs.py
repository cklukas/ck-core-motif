#!/usr/bin/env python3
"""Check drop-site retrieval validates its enclosing widget."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropSMgr.c").read_text(encoding="utf-8")
start = source.index("XmDropSiteRetrieve(")
region = source[start:]
assert "if (enclosingWidget == NULL)" in region
assert region.index("if (enclosingWidget == NULL)") < region.index("_XmWidgetToAppContext(enclosingWidget)")
print("PASS XmDropSiteRetrieve validates null widgets")
