#!/usr/bin/env python3
"""Check spatial growth rejects children without Container constraints."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("RequestSpatialGrowth(\n    Widget")
end = source.index("ConstraintInitialize", start)
region = source[start:end]
lookup = "c = GetContainerConstraint(cwid);"
guard = "if (c == NULL)"
assert lookup in region
assert guard in region
assert region.index(guard) > region.index(lookup)
assert "if (c == NULL)\n\treturn(False);\n    if (CtrSpatialStyleIsNONE(cw))" in region

print("PASS spatial growth rejects children without Container constraints")
