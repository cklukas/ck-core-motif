#!/usr/bin/env python3
"""Check StatusBar item-array allocation failure handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "StatusBar.c").read_text()
start = source.index("void XmStatusBarSetItems")
body = source[start:source.index("Cardinal XmStatusBarGetItemCount", start)]
assert "SIZE_MAX / sizeof(*bar->status_bar.items)" in body
assert "UINT_MAX / sizeof(*bar->status_bar.items)" in body
assert "bar->status_bar.items == NULL" in body
assert "unable to allocate item array" in body
print("PASS StatusBar item-array allocation failure handling")
