#!/usr/bin/env python3
"""Guard MainWindow clip-child compaction and re-evaluation behavior."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "MainW.c").read_text(
    encoding="utf-8"
)
start = text.index("static void \nChangeManaged(")
body = text[start:text.index("\n    if (!XtIsRealized(wid))", start)]
if "for (i = 0; i < cw->composite.num_children; )" not in body:
    raise SystemExit("clip-child traversal does not re-evaluate removed slots")
if "cw->composite.children[j-1] = cw->composite.children[j]" not in body:
    raise SystemExit("clip-child compaction does not shift trailing siblings")
if "cw->composite.children[i-1] = cw->composite.children[i]" in body:
    raise SystemExit("clip-child compaction retains the corrupt fixed-index write")
if "\t\tcontinue;" not in body:
    raise SystemExit("clip-child removal does not revisit the compacted slot")

print("MainWindow clip-child compaction checks passed")
