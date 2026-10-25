#!/usr/bin/env python3
"""Check that _XmGeoReplyYes validates both geometry records."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "GeoUtils.c"
text = source.read_text(encoding="utf-8")
needle = "if(!desired || !response)"
if needle not in text:
    raise SystemExit("geometry reply input guard is missing")

print("geometry reply input guard present")
