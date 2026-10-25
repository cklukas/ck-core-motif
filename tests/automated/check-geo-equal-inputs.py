#!/usr/bin/env python3
"""Check that _XmGeometryEqual validates both geometry inputs."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "GeoUtils.c"
text = source.read_text(encoding="utf-8")
needle = "if(wid == NULL || !geoA || !geoB)"
if needle not in text:
    raise SystemExit("geometry comparison input guard is missing")

print("geometry comparison input guard present")
