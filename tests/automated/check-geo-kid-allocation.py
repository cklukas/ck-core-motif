#!/usr/bin/env python3
"""Check that _XmGetKidGeo validates geometry-list allocation."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "GeoUtils.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "if (wid == NULL)",
    "geo_count > SIZE_MAX / sizeof(XmKidGeometryRec)",
    "XtMalloc(\n\t(Cardinal)(geo_count * sizeof(XmKidGeometryRec)))",
    "if (geo == NULL)",
):
    if needle not in text:
        raise SystemExit(f"kid geometry allocation guard is missing: {needle}")

print("kid geometry allocation guards present")
