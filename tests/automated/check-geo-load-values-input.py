#!/usr/bin/env python3
"""Check that _XmGeoLoadValues validates geometry source/output inputs."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "GeoUtils.c"
text = source.read_text(encoding="utf-8")
needle = """    if (wid == NULL || geoResult == NULL ||
        (wid == instigator && request == NULL))
"""
if needle not in text:
    raise SystemExit("geometry value-loading input guard is missing")

print("geometry value-loading input guard present")
