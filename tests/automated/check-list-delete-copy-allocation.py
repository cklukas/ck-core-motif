#!/usr/bin/env python3
"""Check that XmListDeleteItems guards its temporary copy allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
start = source.index("XmListDeleteItems(")
end = source.index("static void\nAPIDeletePositions", start)
region = source[start:end]
assert "SIZE_MAX / sizeof(XmString)" in region
allocation = region.index("copy = (XmString *)ALLOCATE_LOCAL")
guard = region.index("if (copy == NULL)", allocation)
assert allocation < guard < region.index("for (i = 0; i < item_count", guard)
assert region[guard:].count("_XmAppUnlock(app);") >= 1
print("PASS list deletion copy allocation is guarded")
