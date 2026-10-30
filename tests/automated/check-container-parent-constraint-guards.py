#!/usr/bin/env python3
"""Check Container SetValues handles a missing entry-parent constraint."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("ConstraintSetValues(\n\tWidget")
end = source.index("TestFitItem(", start)
region = source[start:end]
lookup = "pc = GetContainerConstraint(nc->entry_parent);"
guard = "if (pc == NULL)"
assert lookup in region
assert guard in region
assert region.index(guard) > region.index(lookup)
assert region.index(guard) < region.index("nc->depth = pc->depth +1;")
assert "nc->entry_parent = NULL;" in region

print("PASS Container SetValues detaches from missing parent constraints")
