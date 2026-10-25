#!/usr/bin/env python3
"""Guard SelectionBox child-layout traversals against signed indices."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "SelectioB.c").read_text(
    encoding="utf-8"
)
start = text.index("SelectionBoxGeoMatrixCreate(", 10000)
end = text.find("\nstatic ", start + 20)
if end < 0:
    end = len(text)
body = text[start:end]
if body.count("composite.num_children") < 3:
    raise SystemExit("SelectionBox child traversals are missing")
if "int             i;" in body:
    raise SystemExit("SelectionBox uses a signed child index")
if "Cardinal        i;" not in body:
    raise SystemExit("SelectionBox is missing Cardinal child index")

print("SelectionBox child-count bounds checks passed")
