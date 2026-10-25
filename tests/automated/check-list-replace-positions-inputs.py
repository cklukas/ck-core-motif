#!/usr/bin/env python3
"""Check that XmListReplacePositions validates its public inputs first."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
start = source.rindex("XmListReplacePositions(")
end = source.index("static void\nAPISelect", start)
region = source[start:end]
guard = region.index("if (w == NULL || position_list == NULL")
assert "item_count <= 0" in region[guard:]
assert "item_list[i] == NULL" in region[guard:]
assert region.index("_XmWidgetToAppContext(w)") > guard
print("PASS list position replacement validates inputs")
