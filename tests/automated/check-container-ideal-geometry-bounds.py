#!/usr/bin/env python3
"""Check that Container ideal geometry accumulation is saturating."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("GetSize (\n")
end = source.index("/************************************************************************\n * GeometryManager", start)
region = source[start:end]

for needle in (
    "AddPositionSaturated((Position)cw->container.margin_w",
    "AddDimensionSaturated((Dimension)cwid->core.x",
    "cw->container.ideal_height = AddDimensionSaturated",
    "cw->container.ideal_width = AddDimensionSaturated",
):
    if needle not in region:
        raise SystemExit(f"Container ideal geometry is missing: {needle}")

if "ideal_height += desired.height" in region:
    raise SystemExit("Container ideal height still uses wrapping addition")
if "ideal_width += cw->container.margin_w" in region:
    raise SystemExit("Container ideal width still uses wrapping addition")

print("Container ideal geometry uses saturating width, height, and position arithmetic")
