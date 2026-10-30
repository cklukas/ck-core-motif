#!/usr/bin/env python3
"""Check ToggleButtonGadget uses horizontal margins consistently."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/ToggleBG.c").read_text(
    encoding="utf-8"
)

assert "2 * LabG_MarginWidth(new_w)" in source
assert "LabG_MarginWidth(w) - TBG_IndicatorDim(w)" in source
assert "tb->rectangle.x + tb->rectangle.width" not in source
assert "LabG_MarginWidth(tb)" in source
assert "LabG_MarginWidth(newcbox)" in source
print("PASS ToggleButtonGadget margin source contract")
