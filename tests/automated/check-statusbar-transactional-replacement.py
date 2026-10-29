#!/usr/bin/env python3
"""Check StatusBar replacements preserve old items until construction succeeds."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "StatusBar.c").read_text()
start = source.index("void XmStatusBarSetItems")
end = source.index("Cardinal XmStatusBarGetItemCount", start)
body = source[start:end]
assert "XmStatusBarStoredItem *replacement = NULL" in body
assert "replacement = (XmStatusBarStoredItem *)XtCalloc" in body
assert body.index("replacement = (XmStatusBarStoredItem *)XtCalloc") < body.index("old_items = bar->status_bar.items")
assert "release_items(replacement, i + 1, True)" in body
assert "release_items(old_items, old_count, True)" in body
print("PASS StatusBar item replacement is transactional")
