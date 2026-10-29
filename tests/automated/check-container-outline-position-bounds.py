#!/usr/bin/env python3
"""Check that outline-detail positions use saturated signed arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("LayoutOutlineDetail(\n\tWidget\twid)")
region = source[start:source.index("/************************************************************************\n * LayoutSpatial", start)]

for needle in (
    "AdjustPositionSaturated(0,",
    "(int64_t)cw->container.margin_w +",
    "AdjustPositionSaturated(x,",
    "(int64_t)cw->core.width - (int64_t)cw->container.margin_w",
):
    assert needle in region, f"missing bounded outline position arithmetic: {needle}"

assert "x = cw->core.width - cw->container.margin_w" not in region
assert "x = x - (Position)cwid->core.width" not in region
assert "AdjustPositionSaturated((Position)cw->container.margin_w" not in region
print("PASS Container outline-detail positions use saturated signed arithmetic")
