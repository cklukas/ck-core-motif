#!/usr/bin/env python3
"""Check that Container always releases its previous occupancy grid."""

from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.index("PlaceItemReset(", source.index("* PlaceItemReset"))
end = source.index("\n/************************************************************************\n * PlaceCwid", start)
body = source[start:end]

outer = body.index("if (cw->container.cells)")
callback = body.index("container_class.remove_item", outer)
free_cells = body.index("XtFree((char*)cw->container.cells);", outer)
clear_cells = body.index("cw->container.cells = NULL;", free_cells)

assert outer < callback < free_cells < clear_cells
assert "if ((cw->container.cells) &&" not in body

print("Container reset releases an old grid without a remove-item hook")
