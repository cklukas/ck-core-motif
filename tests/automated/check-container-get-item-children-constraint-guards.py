#!/usr/bin/env python3
"""Check public Container child lookup validates constraint retrieval."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("XmContainerGetItemChildren(\n\tWidget")
end = source.index("XmContainerRelayout(\n", start)
region = source[start:end]
lookup = "c = GetContainerConstraint(item);"
guard = "if (c == NULL)"
assert lookup in region
assert guard in region
assert region.index(guard) > region.index(lookup)
assert region.index(guard) < region.index("node = c->node_ptr")

print("PASS public Container child lookup validates constraints")
