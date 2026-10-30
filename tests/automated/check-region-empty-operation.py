#!/usr/bin/env python3
"""Ensure empty region operations clear the overwritten destination."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Region.c").read_text()
start = source.index("static void\nmiRegionOp")
end = source.index("/*======================================================================", start)
region = source[start:end]

empty = region.index("if (max_rects <= 0)")
assert "_XmRegionClear(newReg);" in region[empty:empty + 100]
assert "return;" in region[empty:empty + 140]
overflow = region.index("if ((unsigned long)max_rects >", empty)
assert overflow > empty
print("PASS empty region operation clears destination")
