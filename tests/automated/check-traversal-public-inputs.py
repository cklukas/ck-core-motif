#!/usr/bin/env python3
"""Check traversal public and predicate helpers reject missing widgets."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "Traversal.c").read_text(
    encoding="utf-8"
)
checks = (
    ("_XmIsNavigable(", "wid == NULL"),
    ("_XmWidgetFocusChange(", "wid == NULL"),
    ("_XmSetInitialOfTabGroup(", "tab_group == NULL"),
    ("XmeFocusIsInShell(", "wid == NULL"),
    ("XmIsTraversable(", "wid == NULL"),
    ("XmGetVisibility(", "wid == NULL"),
    ("XmGetTabGroup(", "wid == NULL"),
    ("XmGetFocusWidget(", "wid == NULL"),
)
for name, needle in checks:
    start = text.rfind(name)
    body = text[start : start + 1000]
    if needle not in body:
        raise SystemExit(f"traversal public-input guard is missing: {name}")

print("traversal public-input guards present")
