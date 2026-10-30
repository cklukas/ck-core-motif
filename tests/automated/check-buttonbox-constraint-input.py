#!/usr/bin/env python3
"""Ensure ButtonBox constraint hooks validate their set widget."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/ButtonBox.c").read_text()
init_start = source.index("ConstraintInitialize(Widget request, Widget set,")
init_end = source.index("\n}\n", init_start) + 3
init_body = source[init_start:init_end]
set_start = source.index("ConstraintSetValues(Widget current, Widget request, Widget set,")
set_end = source.index("\n}\n", set_start) + 3
set_body = source[set_start:set_end]

assert "if (set == NULL)" in init_body
assert "if (set == NULL)" in set_body
assert set_body.index("if (set == NULL)") < set_body.index("XmButtonBoxC_pref_width")
print("ButtonBox constraint hooks validate their set widget")
