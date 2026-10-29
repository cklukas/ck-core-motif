#!/usr/bin/env python3
"""Check that Container spatial growth uses bounded arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("static Boolean\nRequestSpatialGrowth(")
end = source.index("/************************************************************************\n * ConstraintInitialize", start)
region = source[start:end]

for needle in (
    "AddDimensionSaturated(\n\t\t    0, (uint64_t)width_in_cells",
    "AddDimensionSaturated(\n\t\t    0, (uint64_t)height_in_cells",
    "geo_desired.width = AddDimensionSaturated",
    "geo_desired.height = AddDimensionSaturated",
    "content_right = (int64_t)cw->core.width",
    "content_bottom = (int64_t)cw->core.height",
    "width_excess = (int64_t)c->user_x - content_right",
    "height_excess = (int64_t)c->user_y - content_bottom",
    "if ((width_excess % cell_width) != 0)",
    "if ((height_excess % cell_height) != 0)",
    "new_width = (int64_t)cwid->core.x",
    "new_height = (int64_t)cwid->core.y",
    "width_increase = AddDimensionSaturated(0,",
    "height_increase = AddDimensionSaturated(0,",
):
    if needle not in region:
        raise SystemExit(f"Container spatial growth is missing: {needle}")

if "width_increase = width_in_cells * cell_width" in region:
    raise SystemExit("Container width growth still multiplies in Dimension arithmetic")
if "height_increase = height_in_cells * cell_height" in region:
    raise SystemExit("Container height growth still multiplies in Dimension arithmetic")
if "geo_desired.width = cw->core.width + width_increase" in region:
    raise SystemExit("Container width request still wraps on addition")
if "c->user_x - (cw->core.width - cw->container.margin_w)" in region:
    raise SystemExit("Container closest width growth still subtracts in Dimension arithmetic")
if "c->user_y - (cw->core.height - cw->container.margin_h)" in region:
    raise SystemExit("Container closest height growth still subtracts in Dimension arithmetic")
if "new_width =  cwid->core.x + cwid->core.width" in region:
    raise SystemExit("Container none-style width growth still sums in mixed-width arithmetic")
if "new_height = cwid->core.y + cwid->core.height" in region:
    raise SystemExit("Container none-style height growth still sums in mixed-width arithmetic")

print("Container spatial growth uses bounded products and request extents")
