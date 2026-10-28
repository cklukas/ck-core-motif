#!/usr/bin/env python3
"""Check List replacement rejects unrepresentable one-based positions."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()
start = source.index("static void\nAPIReplaceItemsPos")
end = source.index("void\nXmListReplaceItems", start)
body = source[start:end]

assert "position > lw->list.itemCount" in body
assert "item_count > lw->list.itemCount - intern_pos" in body
assert "intern_pos + item_count" not in body
print("List replacement position bounds are overflow-safe")
