#!/usr/bin/env python3
"""Check Container preferred-size cell-count arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("GetSpatialSize (", source.index("* GetSpatialSize"))
end = source.index("\n/************************************************************************\n * PlaceItemNone", start)
region = source[start:end]
assert "cell_count != INT_MAX" in region
assert "INT_MAX / cwid_height_in_cells" in region
assert "INT_MAX - cell_count" in region
assert "cell_count = INT_MAX" in region
print("PASS Container cell-count arithmetic saturates safely")
