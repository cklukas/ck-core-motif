#!/usr/bin/env python3
"""Check XmPaned preferred-size accumulation saturates each addition."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Paned.c").read_text()

assert "static Dimension AddPanedDimension(Dimension value, uint64_t addition);" in source
helper = source[source.index("static Dimension\nAddPanedDimension") :]
assert "total > (uint64_t)USHRT_MAX" in helper
preferred = source[source.index("GetPrefSizes(XmPanedWidget pw") : source.index("/*\tFunction Name: RefigureLocations")]
assert preferred.count("AddPanedDimension(sizeused") >= 4
assert "sizeused += (int) pane->size" not in preferred
assert "sizeused += 2*XmPaned_margin" not in preferred
print("PASS XmPaned preferred-size accumulation is bounded")
