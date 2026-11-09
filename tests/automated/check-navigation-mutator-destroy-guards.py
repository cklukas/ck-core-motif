#!/usr/bin/env python3
"""Ensure navigation-widget mutators reject widgets in Xt teardown."""

from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
checks = {
    "TreeView.c": ("void XmTreeViewSetTopRow", "Cardinal XmTreeViewGetTopRow"),
    "BreadcrumbBar.c": ("void XmBreadcrumbBarSetCurrentIndex", "static Boolean BufferedSetValues"),
    "Sidebar.c": ("void XmSidebarToggle", "}"),
}

for filename, (start_marker, end_marker) in checks.items():
    source = (root / filename).read_text()
    start = source.index(start_marker)
    end = source.index(end_marker, start + len(start_marker))
    assert "being_destroyed" in source[start:end], filename

print("PASS navigation mutators reject teardown state")
