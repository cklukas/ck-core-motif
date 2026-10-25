#!/usr/bin/env python3
"""Check that scrolled-list argument merging handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
start = source.index("XmCreateScrolledList(")
end = source.index("/* Selection API", start)
region = source[start:end]
merge = region.index("Args = XtMergeArgLists")
guard = region.index("if (Args == NULL)", merge)
create = region.index("sw = XtCreateManagedWidget", guard)
assert merge < guard < create
assert "DEALLOCATE_LOCAL(s);" in region[guard:create]
assert "return NULL;" in region[guard:create]
print("PASS scrolled-list argument merge failure is guarded")
