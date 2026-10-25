#!/usr/bin/env python3
"""Check Tree loops use full-width Xt count indices."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Tree.c").read_text(
    encoding="utf-8"
)
constraint_start = source.index("ConstraintSetValues(Widget current")
constraint_end = source.index("\n/*", constraint_start)
constraint = source[constraint_start:constraint_end]
assert "Cardinal i;" in constraint
assert "int i;" not in constraint
locations_start = source.index("static void\nCalcLocations(")
locations_end = source.index("\n/*\tFunction Name: RequestNewSize", locations_start)
locations = source[locations_start:locations_end]
assert "register Cardinal i;" in locations
assert "register int      i;" not in locations
print("Tree count loops use full-width Cardinal indices")
