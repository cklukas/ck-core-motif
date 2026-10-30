#!/usr/bin/env python3
"""Check Container anchor toggling validates its constraint record."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("StartSelect(\n        Widget")
end = source.index("SetupDrag(Widget wid", start)
region = source[start:end]
lookup = "c = GetContainerConstraint(cw->container.anchor_cwid);"
guard = "if (c == NULL)"
assert lookup in region
assert guard in region
assert region.index(guard) > region.index(lookup)
assert region.index(guard) < region.index("c->selection_state")

print("PASS Container anchor toggling checks constraints")
