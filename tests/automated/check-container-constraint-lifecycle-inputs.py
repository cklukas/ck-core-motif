#!/usr/bin/env python3
"""Check Container constraint lifecycle validates parent and child state."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
init_start = source.rindex("ConstraintInitialize(")
init_region = source[init_start : source.index("\n}\n", init_start)]
assert "if (cw == NULL || nc == NULL)" in init_region
assert init_region.index("if (ncwid == NULL)") < init_region.index("cw = (XmContainerWidget)")
assert "if (pc == NULL)" in init_region

destroy_start = source.rindex("ConstraintDestroy(")
destroy_region = source[destroy_start : source.index("\n}\n", destroy_start)]
assert destroy_region.index("if (cwid == NULL)") < destroy_region.index("cw = (XmContainerWidget)")
assert "if (cw == NULL || c == NULL)" in destroy_region

print("PASS Container constraint lifecycle validates parent and child state")
