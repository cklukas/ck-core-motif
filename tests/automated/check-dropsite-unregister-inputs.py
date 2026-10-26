#!/usr/bin/env python3
"""Check drop-site unregistration validates its widget."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropSMgr.c").read_text(encoding="utf-8")
start = source.index("XmDropSiteUnregister(")
region = source[start:]
assert "if (widget == NULL)" in region
assert region.index("if (widget == NULL)") < region.index("_XmWidgetToAppContext(widget)")
print("PASS XmDropSiteUnregister validates null widgets")
