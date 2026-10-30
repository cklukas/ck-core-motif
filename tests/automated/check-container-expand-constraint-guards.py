#!/usr/bin/env python3
"""Check Container expand/collapse validates focus constraints."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("ContainerExpandOrCollapse(\n        Widget")
end = source.index("ContainerConvertProc(\n\tWidget", start)
region = source[start:end]
lookup = "c = GetContainerConstraint(focus_cwid);"
guard = "if (c == NULL)"
assert lookup in region
assert guard in region
assert region.index(guard) > region.index(lookup)
assert region.index(guard) < region.index("c->outline_state")

print("PASS Container expand/collapse checks focus constraints")
