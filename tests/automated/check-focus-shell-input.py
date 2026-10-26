#!/usr/bin/env python3
"""Check XmeFocusIsInShell validates its widget before shell lookup."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Traversal.c").read_text(
    encoding="utf-8"
)
start = source.index("\nXmeFocusIsInShell(\n        Widget wid)")
body_start = source.index("{", start)
body_end = source.index("\n}", body_start)
body = source[body_start:body_end]
guard = "if (wid == NULL)\n    return FALSE;"
assert guard in body
assert body.index(guard) < body.index("_XmFindTopMostShell( wid)")
print("focus-shell query guards null widgets before top-level shell lookup")
