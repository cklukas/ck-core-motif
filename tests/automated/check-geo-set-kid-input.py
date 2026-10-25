#!/usr/bin/env python3
"""Check that _XmSetKidGeo validates its geometry-list input."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "GeoUtils.c"
text = source.read_text(encoding="utf-8")
for needle in ("if (kg == NULL)", "return;", "kg[i].kid != NULL"):
    if needle not in text:
        raise SystemExit(f"kid geometry setter input guard is missing: {needle}")

print("kid geometry setter input guard present")
