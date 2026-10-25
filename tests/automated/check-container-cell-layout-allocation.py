#!/usr/bin/env python3
"""Check Container cell-layout occupancy allocation bounds."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("GetSpatialSizeCellAdjustment(", source.index("* GetSpatialSizeCellAdjustment"))
end = source.index("\n/************************************************************************\n * CreateIconHeader", start)
region = source[start:end]
assert "width_in_cells <= 0" in region
assert "SIZE_MAX / (size_t)height_in_cells" in region
assert "cell_count > SIZE_MAX / sizeof(*cell_occupied)" in region
assert "if (cell_occupied == NULL)" in region
assert "width_in_cells == INT_MAX" in region
print("PASS Container cell-layout allocation is bounded")
