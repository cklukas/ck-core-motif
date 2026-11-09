#!/usr/bin/env python3
"""Ensure public composite queries return neutral values during Xt teardown."""

from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
checks = {
    "AutoSuggestBox.c": ("XmAutoSuggestBoxGetSuggestionCount", "XmAutoSuggestBoxDismiss"),
    "BreadcrumbBar.c": ("XmBreadcrumbBarGetSegmentCount", "XmBreadcrumbBarSetCurrentIndex"),
    "Sidebar.c": ("XmSidebarGetContent", "XmSidebarToggle"),
    "Disclosure.c": ("XmDisclosureGetContent", "XmDisclosureSetExpanded"),
    "Flyout.c": ("XmFlyoutIsVisible", "Widget XmCreateFlyout"),
    "TeachingTip.c": ("XmTeachingTipIsVisible", "Widget XmCreateTeachingTip"),
    "SearchBox.c": ("char *XmSearchBoxGetString", "void XmSearchBoxSetString"),
}

for filename, (start_marker, end_marker) in checks.items():
    source = (root / filename).read_text()
    start = source.index(start_marker)
    end = source.index(end_marker, start + len(start_marker))
    assert "being_destroyed" in source[start:end], filename

print("PASS public composite queries reject teardown state")
