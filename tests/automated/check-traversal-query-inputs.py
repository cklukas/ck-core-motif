#!/usr/bin/env python3
"""Check traversal query helpers validate widget ancestry."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "Traversal.c").read_text(
    encoding="utf-8"
)
for name, needle in (("static Boolean\nCallTraverseObsured(", "new_focus == NULL"),
                     ("void \n_XmClearFocusPath(", "wid == NULL"),
                     ("unsigned char \n_XmGetFocusPolicy(", "w == NULL")):
    start = text.index(name)
    body = text[start : start + 1200]
    if needle not in body:
        raise SystemExit(f"traversal query guard is missing: {name}")

print("traversal query input guards present")
