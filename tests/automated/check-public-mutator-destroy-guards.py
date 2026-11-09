#!/usr/bin/env python3
"""Check public composite mutators avoid Xt teardown state."""

from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
checks = {
    "RowActions.c": ("XmRowActionsReveal", "Boolean XmRowActionsIsRevealed"),
    "Disclosure.c": ("XmDisclosureSetExpanded", "}"),
    "TeachingTip.c": ("XmTeachingTipHide", "Boolean XmTeachingTipIsVisible"),
    "Flyout.c": ("XmFlyoutHide", "Boolean XmFlyoutIsVisible"),
}

for filename, (start_marker, end_marker) in checks.items():
    source = (root / filename).read_text()
    start = source.index(start_marker)
    end = source.index(end_marker, start + len(start_marker))
    body = source[start:end]
    assert "being_destroyed" in body, filename

print("PASS public composite mutators reject teardown state")
