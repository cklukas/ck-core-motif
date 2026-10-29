#!/usr/bin/env python3
"""Check that Container icon-width resizing uses signed edge arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("ResizeIconWidths(")
region = source[start:source.index("/************************************************************************\n * Layout (Private Function)", start)]

for needle in (
    "int64_t max_x = 0",
    "(int64_t)cw->container.ideal_width -",
    "edge_x = (int64_t)cwid->core.x + (int64_t)desired.width",
    "available_width = max_x > (int64_t)cwid->core.x ?",
    "cwid->core.width = max_x > (int64_t)cwid->core.x ?",
):
    assert needle in region, f"missing bounded icon-width calculation: {needle}"

assert "max_x - cwid->core.x" not in region
print("PASS Container icon-width resizing bounds signed edge arithmetic")
