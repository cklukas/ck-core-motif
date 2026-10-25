#!/usr/bin/env python3
"""Check List click handling guards missing internal item state."""

from pathlib import Path


source = Path("lib/Xm/List.c").read_text()
start = source.rindex("ClickElement(XmListWidget")
end = source.index("\n/************************************************************************", start)
region = source[start:end]

assert "lw->list.itemCount == 0" in region
assert "lw->list.InternalList == NULL" in region
assert "assert(lw->list.itemCount && lw->list.InternalList);" not in region

print("List click handling guards missing internal state")
