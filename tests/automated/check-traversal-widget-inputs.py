#!/usr/bin/env python3
"""Check that traversal widget lifecycle hooks reject missing widgets."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "Traversal.c").read_text(
    encoding="utf-8"
)
for name in ("_XmNavigInitialize(", "_XmNavigSetValues(",
             "XmeNavigChangeManaged(", "_XmNavigResize("):
    start = text.index(name)
    body = text[start : text.index("}\n", start) + 2]
    if "== NULL" not in body:
        raise SystemExit(f"traversal widget guard is missing: {name}")

print("traversal widget input guards present")
