#!/usr/bin/env python3
"""Check Container none-style placement validates child constraints."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("PlaceItemNone(\n    Widget")
end = source.index("PlaceItemGridCells(\n    Widget", start)
region = source[start:end]
lookup = "XmContainerConstraint\tc = GetContainerConstraint(cwid);"
guard = "if (c == NULL)"
assert lookup in region
assert guard in region
assert region.index(guard) > region.index(lookup)
assert region.index(guard) < region.index("PlaceCwid")

print("PASS Container none-style placement checks child constraints")
