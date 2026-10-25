#!/usr/bin/env python3
"""Check that Paned sash/separator list allocation fails safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Paned.c").read_text(encoding="utf-8")
start = source.index("ManageAndUnmanageSashs(XmPanedWidget pw)")
end = source.index("/*\tFunction Name: CreateSeparator", start)
region = source[start:end]
assert "if (alloc_size == 0)" in region
assert "pw->composite.num_children > UINT_MAX / sizeof(Widget)" in region
assert "sizeof(Widget) * pw->composite.num_children / 2" not in region
assert "if (managed_sashs == NULL || unmanaged_sashs == NULL" in region
assert region.count("XtFree((char *)") >= 4
print("PASS Paned sash/separator list allocations are guarded")
