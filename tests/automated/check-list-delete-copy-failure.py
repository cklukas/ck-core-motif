#!/usr/bin/env python3
"""Check XmListDeleteItems aborts safely if an item copy fails."""

from pathlib import Path


source = Path("lib/Xm/List.c").read_text()
start = source.index("/* Make a copy of items in case of XmNitems from w */")
end = source.index("\n\n  DrawHighlight(lw", start)
body = source[start:end]

assert "copy[i] = XmStringCopy(items[i]);" in body
assert "if (copy[i] == NULL)" in body
assert "XmStringFree(copy[--i]);" in body
assert "DEALLOCATE_LOCAL((char *)copy);" in body
assert "_XmAppUnlock(app);" in body
assert "return;" in body

print("XmListDeleteItems handles failed item copies transactionally")
