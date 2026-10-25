#!/usr/bin/env python3
"""Guard shared geometry/tracking child traversals against signed indices."""

from pathlib import Path

for name, needles in (
    ("GMUtils.c", ("composite.num_children", "Cardinal i")),
    ("TrackLoc.c", ("composite.num_children", "Cardinal i;")),
):
    text = (Path(__file__).parents[2] / "lib" / "Xm" / name).read_text(
        encoding="utf-8"
    )
    if text.count("composite.num_children") == 0:
        raise SystemExit(f"{name} has no child traversal")
    if "int i" in text or "register int i" in text:
        raise SystemExit(f"{name} retains a signed child index")
    for needle in needles:
        if needle not in text:
            raise SystemExit(f"{name} missing {needle}")

print("Geometry/tracking child-count bounds checks passed")
