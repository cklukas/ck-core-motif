#!/usr/bin/env python3
"""Check the drop-site registration mutator validates its widget."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropSMgr.c").read_text(encoding="utf-8")
start = source.index("XmDropSiteRegister(")
region = source[start:]
assert "if (widget == NULL)" in region
assert region.index("if (widget == NULL)") < region.index("_XmWidgetToAppContext(widget)")
print("PASS XmDropSiteRegister validates null widgets")
