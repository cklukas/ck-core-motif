#!/usr/bin/env python3
"""Check Container cell conversion and closest placement bounds."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()

place_start = source.index("PlaceItemGridCells(\n    Widget")
place_end = source.index("/************************************************************************\n * GetCellFromCoord", place_start)
place = source[place_start:place_end]
assert "if (closest_cell < 0)" in place
assert "if (trial_cell < 0 || trial_cell >= cw->container.cell_count)" in place

coord_start = source.index("/************************************************************************\n * GetCoordFromCell")
coord_end = source.index("PlaceItemReset(", coord_start)
coord = source[coord_start:coord_end]
assert "cw->container.cell_count <= 0" in coord
assert "cell_idx < 0 || cell_idx >= cw->container.cell_count" in coord

snap_start = source.index("/************************************************************************\n * SnapCwid")
snap = source[snap_start:]
assert "if (point == NULL)" in snap
assert "target_cell < 0 || target_cell >= cw->container.cell_count" in snap
print("PASS Container cell conversion rejects invalid indices")
