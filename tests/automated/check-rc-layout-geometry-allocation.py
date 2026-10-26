#!/usr/bin/env python3
"""Check that RowColumn geometry-array sizing is overflow-safe."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/RCLayout.c").read_text(
    encoding="utf-8"
)

assert "kid_count > SIZE_MAX - 1 - (size_t)tocFound" in source
assert "geo_count > UINT_MAX / sizeof(XmRCKidGeometryRec)" in source
assert "if (geo == NULL)" in source
print("RowColumn geometry allocation is bounded")
