#!/usr/bin/env python3
"""Check that dynamic Container tabs roll back partial XmTab creation."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "Container.c"
text = source.read_text()

start = text.index("GetDumbTabList(\n    int         tab_size")
end = text.index("GetDynFirstColWidth", start)
block = text[start:end]

if "if (tab_pool[i] == NULL)" not in block:
    raise SystemExit("XmTabCreate failure is not checked")
if "XmTabFree(tab_pool[i]);" not in block:
    raise SystemExit("partial XmTab entries are not released")
if "XtFree((char *)tab_pool);" not in block:
    raise SystemExit("tab pool is not released on XmTabCreate failure")
if "return NULL;" not in block:
    raise SystemExit("XmTabCreate failure does not fail closed")

print("Container dynamic tab creation rolls back allocation failures")
