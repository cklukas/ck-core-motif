#!/usr/bin/env python3
"""Check Container sizing skips nodes without closest-layout constraints."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("GetSpatialSize (\n        Widget")
end = source.index("PlaceItemNone(", start)
region = source[start:end]
lookup = "c = GetContainerConstraint(node->widget_ptr);"
guard = "if (c == NULL)"
assert lookup in region
assert guard in region
assert region.index(guard) > region.index(lookup)
assert region.index(guard) < region.index("c->user_x")
assert "node = GetNextNode(node);" in region[region.index(guard):]

print("PASS Container sizing skips nodes without closest-layout constraints")
