#!/usr/bin/env python3
"""Check Container cell-grid reset bounds and allocation handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("PlaceItemReset(", source.index("* PlaceItemReset"))
end = source.index("\n/************************************************************************\n * PlaceCwid", start)
region = source[start:end]
assert "cell_width == 0 || cell_height == 0" in region
assert "width_in_cells > INT_MAX / height_in_cells" in region
assert "if (cw->container.cells == NULL)" in region
assert "current_width_in_cells = 0" in region
print("PASS Container cell-grid reset checks metrics and allocation")
