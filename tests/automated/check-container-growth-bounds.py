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
):
    if needle not in region:
        raise SystemExit(f"Container spatial growth is missing: {needle}")

if "width_increase = width_in_cells * cell_width" in region:
    raise SystemExit("Container width growth still multiplies in Dimension arithmetic")
if "height_increase = height_in_cells * cell_height" in region:
    raise SystemExit("Container height growth still multiplies in Dimension arithmetic")
if "geo_desired.width = cw->core.width + width_increase" in region:
    raise SystemExit("Container width request still wraps on addition")

print("Container spatial growth uses bounded products and request extents")
