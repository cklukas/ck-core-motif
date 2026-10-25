#!/usr/bin/env python3
"""Check that _XmMakeGeometryRequest validates Xt request inputs."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "GeoUtils.c"
text = source.read_text(encoding="utf-8")
needle = "if (w == NULL || geom == NULL)"
if needle not in text:
    raise SystemExit("geometry request input guard is missing")

print("geometry request input guard present")
