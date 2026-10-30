#!/usr/bin/env python3
"""Check Container SetValues skips children without constraints."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("SetValues(\n    Widget")
end = source.index("ConstraintInitialize(", start)
region = source[start:end]
lookup = "c = GetContainerConstraint(cwid);"
guard = "if (c == NULL)"
assert lookup in region
assert guard in region
assert region.index(guard) > region.index(lookup)
assert region.index(guard) < region.index("if (!CtrICON(cwid) || (c->entry_parent))")

print("PASS Container SetValues skips children without constraints")
