#!/usr/bin/env python3
"""Check that Container CELLS geometry rejects zero cell dimensions."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("static Boolean\nRequestSpatialGrowth(")
end = source.index("/************************************************************************\n * ConstraintInitialize", start)
region = source[start:end]
width_guard = region.index("if (cell_width == 0)")
height_guard = region.index("if (cell_height == 0)")
width_division = region.index("width_in_cells = cwid->core.width / cell_width")
height_division = region.index("height_in_cells = cwid->core.height / cell_height")
if width_guard > width_division or height_guard > height_division:
    raise SystemExit("Container CELLS division occurs before the zero-size guard")
if "cell_width = 0,cell_height = 0" not in region:
    raise SystemExit("Container cell dimensions are not initialized defensively")

print("Container CELLS geometry rejects zero cell dimensions before division")
