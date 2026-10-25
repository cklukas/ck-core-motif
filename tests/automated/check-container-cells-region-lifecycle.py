#!/usr/bin/env python3
"""Check Container CELLS region allocation and teardown guards."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()

destroy_start = source.index("Destroy(")
destroy_end = source.index("CreateWidget(", destroy_start)
destroy = source[destroy_start:destroy_end]
assert "if (cw->container.cells_region != NULL)" in destroy
assert "XDestroyRegion(cw->container.cells_region);" in destroy

fit_start = source.index("TestFitItem(\n    Widget")
fit_end = source.index("SetValues(", fit_start)
fit = source[fit_start:fit_end]
assert "CtrSpatialStyleIsCELLS(cw)" in fit
assert "cw->container.cells_region == NULL" in fit
assert "return(False);" in fit

place_start = source.index("PlaceItemGridCells(\n    Widget")
place_end = source.index("GetCellFromCoord(", place_start)
place = source[place_start:place_end]
assert "CtrSpatialStyleIsCELLS(cw)" in place
assert "cw->container.cells_region == NULL" in place

reset_start = source.index("PlaceItemReset(\n    Widget")
reset_end = source.index("PlaceCwid(", reset_start)
reset = source[reset_start:reset_end]
assert "if (cw->container.cells_region != NULL)" in reset
print("PASS Container CELLS region lifecycle handles allocation failure")
