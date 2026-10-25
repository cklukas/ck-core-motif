#!/usr/bin/env python3
"""Guard FileSelectionBox child-layout traversals against signed indices."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text(
    encoding="utf-8"
)
start = text.index("FileSBGeoMatrixCreate(", 10000)
end = text.index("\nstatic ", start + 20)
body = text[start:end]
if body.count("composite.num_children") < 3:
    raise SystemExit("FileSelectionBox child traversals are missing")
if "int             i;" in body:
    raise SystemExit("FileSelectionBox uses a signed child index")
if "Cardinal        i;" not in body:
    raise SystemExit("FileSelectionBox is missing Cardinal child index")

print("FileSelectionBox child-count bounds checks passed")
