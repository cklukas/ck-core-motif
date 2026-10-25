#!/usr/bin/env python3
"""Check RowColumn traversal child-list sizing and publication."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowColumn.c").read_text(
    encoding="utf-8"
)
start = source.index("TraversalChildren (")
start = source.index("{", start)
end = source.index("\n}\n", start) + 2
region = source[start:end]
for needle in (
    "child_count == (Cardinal)~0",
    "SIZE_MAX / sizeof(Widget) - 1",
    "(size_t)((Cardinal)~0) / sizeof(Widget) - 1",
    "new_children = (WidgetList) XtMalloc",
    "if (new_children == NULL)",
    "*childList = new_children;",
    "Cardinal i;",
):
    assert needle in region, needle
assert "*childList = NULL;" in region
assert "*numChildren = 0;" in region
print("RowColumn traversal child-list sizing and publication are guarded")
