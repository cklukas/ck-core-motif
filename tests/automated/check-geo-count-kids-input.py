#!/usr/bin/env python3
"""Check that _XmGeoCount_kids tolerates partial composite state."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "GeoUtils.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "if (c == NULL ||",
    "c->composite.children == NULL",
    "c->composite.children[i] != NULL",
):
    if needle not in text:
        raise SystemExit(f"kid-count input guard is missing: {needle}")

print("kid-count input guards present")
