#!/usr/bin/env python3
"""Check that XmListDeleteItem validates its public inputs first."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
start = source.index("XmListDeleteItem(")
end = source.index("static void\nCleanUpList", start)
region = source[start:end]
guard = region.index("if (w == NULL || item == NULL)")
assert region.index("_XmWidgetToAppContext(w)") > guard
assert region.index("ItemNumber(lw, item)") > guard
print("PASS list item deletion validates widget and item inputs")
