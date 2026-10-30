#!/usr/bin/env python3
"""Check Container marked-state synchronization skips missing constraints."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("SetMarkedCwids(\n\tWidget")
end = source.index("ResetMarkedCwids(\n\tWidget", start)
region = source[start:end]
lookup = "c = GetContainerConstraint(node->widget_ptr);"
guard = "if (c == NULL)"
assert lookup in region
assert guard in region
assert region.index(guard) > region.index(lookup)
assert region.index(guard) < region.index("c->selection_state")

print("PASS Container marked-state synchronization skips missing constraints")
