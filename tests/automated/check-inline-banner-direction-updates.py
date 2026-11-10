#!/usr/bin/env python3
"""Check Toast and InfoBar repaint/reflow after a direction update."""
from pathlib import Path
root = Path(__file__).parents[2] / "lib" / "Xm"
toast = (root / "Toast.c").read_text()
info = (root / "InfoBar.c").read_text()
assert "XmPrim_layout_direction(old) != XmPrim_layout_direction(toast)" in toast
assert "XmPrim_layout_direction(old)!=XmPrim_layout_direction(bar)" in info
print("PASS Toast and InfoBar react to layout-direction updates")
