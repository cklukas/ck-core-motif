#!/usr/bin/env python3
"""Check Container occupancy accesses validate the grid and cell index."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()

remove = source[source.index("RemoveItem("):source.index("GetSpatialSize (")]
assert "cw->container.cells != NULL" in remove
assert "c->cell_idx >= 0" in remove
assert "c->cell_idx < cw->container.cell_count" in remove

place_start = source.index("PlaceItemGridCells(\n    Widget")
place = source[place_start:source.index("GetCellFromCoord(", place_start)]
assert "cw->container.cells == NULL || cw->container.cell_count <= 0" in place

fit_start = source.index("TestFitItem(\n    Widget")
fit = source[fit_start:source.index("SetValues(", fit_start)]
assert "trial_cell < 0 || trial_cell >= cw->container.cell_count" in fit
assert "cw->container.cells == NULL" in fit
print("PASS Container occupancy accesses validate grid and cell indices")
