#!/usr/bin/env python3
"""Check ToggleButtonGadget text origins clamp legacy Position fields."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleBG.c").read_text(
    encoding="utf-8"
)
initialize = source[source.index("static void \nInitialize("):source.index("static void \nDestroy(")]
compute_space = source[source.index("static void \nComputeSpace("):source.index("/*************************************<->", source.index("static void \nComputeSpace("))]

assert initialize.count("LabG_AccTextRect(new_w).x = togglebg_bounded_position(") == 2
assert "LabG_TextRect(new_w).y = togglebg_bounded_position(" in initialize
assert "(int64_t)new_w->rectangle.height" in initialize
assert "TBG_IndTopDelta(new_w) = new_margin - old_margin;" in initialize
assert "TBG_IndRightDelta(new_w) = new_margin - old_margin;" in initialize
assert "TBG_IndLeftDelta(new_w) = new_margin - old_margin;" in initialize
assert "new_w->rectangle.width = togglebg_bounded_dimension(" in initialize
assert "new_w->rectangle.height = togglebg_bounded_dimension(" in initialize
assert "LabG_TextRect(tb).y = togglebg_bounded_position(" in compute_space
assert compute_space.count("LabG_TextRect(tb).x = togglebg_bounded_position(") == 2
print("PASS ToggleButtonGadget text origins clamp legacy Position fields")
