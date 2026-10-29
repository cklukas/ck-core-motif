#!/usr/bin/env python3
"""Check status-bar replacement validates input before releasing old items."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "StatusBar.c").read_text()
start = source.index("void XmStatusBarSetItems")
end = source.index("Cardinal XmStatusBarGetItemCount", start)
function = source[start:end]
guard = function.index("SIZE_MAX / sizeof(*bar->status_bar.items)")
replacement = function.index("replacement = (XmStatusBarStoredItem *)XtCalloc")
release = function.index("release_items(old_items, old_count, True)")
assert guard < replacement < release
assert "(count && !items)" in function
print("StatusBar validates replacements before releasing existing items")
