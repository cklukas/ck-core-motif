#!/usr/bin/env python3
"""Check collection-style widget queries reject teardown state."""

from pathlib import Path


root = Path(__file__).parents[2]
expectations = {
    "lib/Xm/RowActions.c": ("XmRowActionsGetItemCount", "XmRowActionsIsRevealed"),
    "lib/Xm/CommandPalette.c": (
        "XmCommandPaletteGetItemCount", "XmCommandPaletteGetVisibleCount",
        "XmCommandPaletteGetFilter", "XmCommandPaletteIsShown",
    ),
    "lib/Xm/StatusBar.c": (
        "XmStatusBarGetItemCount", "XmStatusBarGetVisibleItemCount",
        "XmStatusBarGetOverflowCount",
    ),
    "lib/Xm/Toolbar.c": (
        "XmToolbarGetActionCount", "XmToolbarGetVisibleActionCount",
        "XmToolbarGetOverflowCount",
    ),
    "lib/Xm/SegmentedControl.c": (
        "XmSegmentedControlGetItemCount", "XmSegmentedControlGetSelectedIndex",
        "XmSegmentedControlGetItemId",
    ),
    "lib/Xm/TreeView.c": (
        "XmTreeViewGetNodeCount", "XmTreeViewGetSelectedId", "XmTreeViewGetTopRow",
    ),
}

for relative, functions in expectations.items():
    text = (root / relative).read_text(encoding="utf-8")
    for function in functions:
        start = text.index(function)
        assert "widget->core.being_destroyed" in text[start:start + 500], (
            relative, function
        )

print("collection widget queries guard teardown state")
