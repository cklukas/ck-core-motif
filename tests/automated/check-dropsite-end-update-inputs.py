#!/usr/bin/env python3
"""Check drop-site update completion validates its reference widget."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropSMgr.c").read_text(encoding="utf-8")
start = source.index("XmDropSiteEndUpdate(")
region = source[start:]
assert "if (refWidget == NULL)" in region
assert region.index("if (refWidget == NULL)") < region.index("_XmWidgetToAppContext(refWidget)")
print("PASS XmDropSiteEndUpdate validates null widgets")
