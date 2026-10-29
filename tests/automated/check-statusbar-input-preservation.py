#!/usr/bin/env python3
"""Check oversized status-bar input is rejected before existing items are freed."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "StatusBar.c").read_text()
start = source.index("void XmStatusBarSetItems")
end = source.index("Cardinal XmStatusBarGetItemCount", start)
function = source[start:end]
guard = function.index("SIZE_MAX / sizeof(*bar->status_bar.items)")
free = function.index("free_items(bar, True)")
assert guard < free
assert "(count && !items)" in function
print("StatusBar rejects oversized item arrays before replacing existing items")
