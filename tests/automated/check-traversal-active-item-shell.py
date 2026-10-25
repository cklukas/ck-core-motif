#!/usr/bin/env python3
"""Check traversal helpers validate widget and top-shell resolution."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "Traversal.c").read_text(
    encoding="utf-8"
)
start = text.index("_XmGetActiveItem(")
body = text[start : text.index("}\n", start) + 2]
if "w == NULL" not in body:
    raise SystemExit("active-item widget guard is missing")
start = text.index("static Boolean\nCallFocusMoved(")
body = text[start : text.index("}\n", start) + 2]
if "topShell == NULL" not in body:
    raise SystemExit("focus-moved top-shell guard is missing")

print("traversal active-item and shell guards present")
