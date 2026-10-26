#!/usr/bin/env python3
"""Check Container cell coordinate conversion guards."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("GetCellFromCoord(", source.index("* GetCellFromCoord"))
end = source.index("\n/************************************************************************\n * PlaceItemReset", start)
region = source[start:end]
assert "cell_width <= 0 || cell_height <= 0" in region
assert "return NO_CELL;" in region
assert "if (point == NULL)" in region
assert "current_width_in_cells <= 0" in region
coord_start = source.index("GetCoordFromCell(", source.index("* GetCoordFromCell"))
coord_end = source.index("/************************************************************************\n * PlaceItemReset", coord_start)
coord_region = source[coord_start:coord_end]
assert "AddPositionSaturated(0" in coord_region
assert "(uint64_t)col * (uint64_t)cell_width" in coord_region
assert "(uint64_t)row * (uint64_t)cell_height" in coord_region
print("PASS Container cell coordinate conversion handles incomplete grids")
