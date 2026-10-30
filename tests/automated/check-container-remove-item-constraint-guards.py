#!/usr/bin/env python3
"""Check Container removal validates child constraints."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("RemoveItem(\n    Widget")
end = source.index("GetSpatialSize (\n        Widget", start)
region = source[start:end]
lookup = "XmContainerConstraint c = GetContainerConstraint(cwid);"
guard = "if (c == NULL)"
assert lookup in region
assert guard in region
assert region.index(guard) > region.index(lookup)
assert region.index(guard) < region.index("CtrItemIsPlaced")

print("PASS Container removal checks child constraints")
