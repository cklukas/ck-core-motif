#!/usr/bin/env python3
"""Check that List insertion normalizes stale single-selection state."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()
start = source.index("APIAddItems(XmListWidget", 100000)
start = source.index("{", start)
end = source.index("\n}\n", start)
body = source[start:end]

assert "selectedPositionCount > 1" in body
assert "selectedPositions != NULL" in body
assert "UpdateSelectedList(lw, TRUE);" in body
assert "assert(lw->list.selectedPositionCount <= 1)" not in body

print("List single-selection normalization: ok")
