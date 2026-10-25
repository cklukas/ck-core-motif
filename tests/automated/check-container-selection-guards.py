#!/usr/bin/env python3
"""Check Container selection callback arrays handle allocation failures."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "Container.c"
text = source.read_text()
start = text.index("static  WidgetList\nGetSelectedCwids")
end = text.index("/*ARGSUSED*/\nstatic\tvoid\nGetSelectedItems", start)
block = text[start:end]
if "SIZE_MAX / sizeof(Widget)" not in block:
    raise SystemExit("selected-item size overflow is not guarded")
if "if (selected_items == NULL)" not in block:
    raise SystemExit("selected-item allocation is not guarded")
if "XtFree((char *)selected_items);" not in block:
    raise SystemExit("incomplete selected-item traversal is not freed")

if text.count("if (cbs.selected_items != NULL)") < 2:
    raise SystemExit("single-selection callback allocations are not guarded")

print("Container selection callback arrays guard allocation failures")
