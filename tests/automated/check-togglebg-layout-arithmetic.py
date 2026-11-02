#!/usr/bin/env python3
"""Check ToggleButtonGadget layout arithmetic uses signed dimensions."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleBG.c").read_text()

assert "((int)tb->rectangle.height - (int)onH) / 2" in source
assert "LabG_AccTextRect(new_w).x = togglebg_bounded_position(" in source
assert "(int64_t)new_w->rectangle.height - LabG_MarginTop" in source
assert "(int64_t)tb->rectangle.height - LabG_MarginTop" in source

print("ToggleButtonGadget layout geometry uses signed arithmetic")
