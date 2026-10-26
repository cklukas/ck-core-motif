#!/usr/bin/env python3
"""Check that Container spatial-size extents cannot wrap Dimension."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("static  void\nGetSpatialSize (")
end = source.index("/************************************************************************\n * PlaceItemNone", start)
region = source[start:end]

for needle in (
    "AddDimensionSaturated(\n\t\t(Dimension)MAX(0, node->widget_ptr->core.x)",
    "AddDimensionSaturated((Dimension)MAX(0, c->user_x)",
    "trial_width = AddDimensionSaturated(trial_width",
    "width = AddDimensionSaturated(0",
    "height = AddDimensionSaturated(0",
):
    if needle not in region:
        raise SystemExit(f"Container spatial size is missing: {needle}")

if "trial_width += 2 * cw->container.margin_w" in region:
    raise SystemExit("Container spatial width still wraps margin arithmetic")
if "width = width_in_cells * cell_width" in region:
    raise SystemExit("Container spatial width still wraps cell multiplication")

print("Container spatial-size extents use saturating Dimension arithmetic")
