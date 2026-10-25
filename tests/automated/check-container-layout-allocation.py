#!/usr/bin/env python3
"""Check Container outline layout allocation fallback."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.rindex("LayoutOutlineDetail(")
end = source.index("\n/************************************************************************\n * LayoutSpatial", start)
region = source[start:end]
assert "Boolean             outline_ready = True;" in region
assert "new_outline_segs != NULL" in region
assert "cw->container.outline_segs = new_outline_segs;" in region
assert "outline_ready = False;" in region
assert "point_at_depth == NULL" in region
assert "if (outline_ready)" in region
print("PASS Container outline layout handles geometry allocation failure")
