#!/usr/bin/env python3
"""Check Container removal cannot make an occupancy count negative."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("RemoveItem(\n    Widget")
end = source.index("GetSpatialSize (", start)
region = source[start:end]
assert "cw->container.cells != NULL" in region
assert "c->cell_idx >= 0" in region
assert "c->cell_idx < cw->container.cell_count" in region
assert "cw->container.cells[c->cell_idx] > 0" in region
print("PASS Container removal preserves non-negative occupancy counts")
