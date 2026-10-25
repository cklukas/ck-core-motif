#!/usr/bin/env python3
"""Check legacy PanedWindow managed-child growth is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Paned.c").read_text(
    encoding="utf-8"
)
start = source.index("ReManageChildren(XmPanedWidget pw)")
end = source.index("/*\tFunction Name: ChangeManaged", start)
region = source[start:end]
for needle in (
    "Cardinal i;",
    "new_slots = XmPaned_num_slots(pw) + BLOCK;",
    "SIZE_MAX / sizeof(Widget)",
    "(size_t)((Cardinal)~0) / sizeof(Widget)",
    "new_children = (WidgetList)XtRealloc",
    "if (new_children == NULL)",
    "XmPaned_managed_children(pw) = new_children;",
):
    assert needle in region, needle
assert "XmPaned_num_slots(pw) += BLOCK" not in region
print("legacy PanedWindow managed-child growth is transactional")
