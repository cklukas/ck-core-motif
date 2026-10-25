#!/usr/bin/env python3
"""Check that _XmGeoSetupKid validates its destination geometry."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "GeoUtils.c"
text = source.read_text(encoding="utf-8")
needle = """    if (geo == NULL)
        return FALSE;
    if(    !kidWid"""
if needle not in text:
    raise SystemExit("geometry setup destination guard is missing")

print("geometry setup destination guard present")
