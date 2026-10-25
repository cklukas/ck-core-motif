#!/usr/bin/env python3
"""Check that XmListDeleteItems validates its public inputs first."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
start = source.index("XmListDeleteItems(")
end = source.index("static void\nAPIDeletePositions", start)
region = source[start:end]
guard = region.index("if (w == NULL || items == NULL || item_count <= 0)")
assert region.index("_XmWidgetToAppContext(w)") > guard
assert region.index("old_max_height = lw->list.MaxItemHeight") > guard
print("PASS list deletion validates widget and count inputs")
