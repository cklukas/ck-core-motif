#!/usr/bin/env python3
"""Check that XmStatusBar handles layout scratch allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "StatusBar.c").read_text()
start = source.index("static void layout(XmStatusBarWidget bar)")
end = source.index("static void negotiate", start)
body = source[start:end]

assert "bar->status_bar.item_count && !chosen" in body
assert "visible_count = 0" in body
assert "overflow_count = bar->status_bar.item_count" in body
assert "bar->status_bar.laying_out = False" in body
assert "return;" in body
print("PASS XmStatusBar layout allocation failure is contained")
