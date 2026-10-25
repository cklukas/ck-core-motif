#!/usr/bin/env python3
"""Check that traversal focus entry points reject missing widgets."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "Traversal.c").read_text(
    encoding="utf-8"
)
for name in ("void \n_XmValidateFocus(", "void \n_XmNavigDestroy(",
             "Boolean \n_XmMgrTraversal("):
    start = text.index(name)
    body = text[start : text.index("}\n", start) + 2]
    if "wid == NULL" not in body:
        raise SystemExit(f"focus-entry guard is missing: {name}")
start = text.index("static Boolean\nCallFocusMoved(")
body = text[start : text.index("}\n", start) + 2]
if "old == NULL && new_wid == NULL" not in body:
    raise SystemExit("focus-moved empty-input guard is missing")

print("traversal focus entry guards present")
