#!/usr/bin/env python3
"""Check guarded growth of the cardinal-list resource converter."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text(encoding="utf-8")

if "crd_array_size > INT_MAX / 2" not in text:
    raise SystemExit("cardinal-list doubling guard is missing")
if "SIZE_MAX / sizeof(Cardinal)" not in text:
    raise SystemExit("cardinal-list allocation-size guard is missing")
if "UINT_MAX / sizeof(Cardinal)" not in text:
    raise SystemExit("cardinal-list Xt allocation-size guard is missing")
if "(Cardinal)(sizeof(Cardinal) *" not in text:
    raise SystemExit("cardinal-list allocation width is implicit")
if "goto invalid_cardinal;" not in text:
    raise SystemExit("cardinal-list failure does not use cleanup path")

print("cardinal-list growth is bounded")
