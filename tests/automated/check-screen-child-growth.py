#!/usr/bin/env python3
"""Check XmScreen child-array growth preserves state on allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c").read_text()
start = source.index("static void \nInsertChild(")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "new_children = (WidgetList)XtRealloc" in region
assert "if (new_children == NULL)" in region
assert region.index("if (new_children == NULL)") < region.index("cw->desktop.children = children")
assert "(size_t)new_slots > SIZE_MAX / sizeof(*new_children)" in region
assert "cw->desktop.num_slots +=" not in region
assert "cw->desktop.children = children =" not in region
print("PASS XmScreen child-growth transaction")
