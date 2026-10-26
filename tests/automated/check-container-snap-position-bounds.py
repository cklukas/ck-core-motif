#!/usr/bin/env python3
"""Check that Container snapping cannot wrap XPoint coordinates."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("SnapCwid(\n")
end = source.index("/************************************************************************\n * HideCwid", start)
region = source[start:end]

if region.count("AdjustPositionSaturated") < 6:
    raise SystemExit("Container snapping does not saturate all signed adjustments")
for needle in (
    "cell_extent = (int64_t)width_in_cells",
    "cell_extent = (int64_t)height_in_cells",
    "Position\t\tlimit",
):
    if needle not in region:
        raise SystemExit(f"Container snapping is missing: {needle}")
if "point->x +=" in region or "point->y +=" in region:
    raise SystemExit("Container snapping still uses wrapping coordinate additions")

print("Container snapping uses saturating signed position adjustments")
