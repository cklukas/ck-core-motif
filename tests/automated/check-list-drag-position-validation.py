#!/usr/bin/env python3
"""Check List drag conversion filters stale selected positions safely."""

from pathlib import Path


source = Path("lib/Xm/List.c").read_text()
start = source.index("/* The selectedItems array may contain extraneous entries. */")
end = source.index("\n\n      if (cs->target == atoms[XmACOMPOUND_TEXT]", start)
body = source[start:end]

assert "concat = NULL;" in body
assert "items != NULL && pos != NULL" in body
assert "pos[i] <= 0 || pos[i] > lw->list.itemCount" in body
assert "items[pos[i] - 1] == NULL" in body
assert "XmStringCopy(items[pos[i] - 1])" in body
assert "XmStringFree(concat);" in body

print("XmList drag conversion validates selected positions")
