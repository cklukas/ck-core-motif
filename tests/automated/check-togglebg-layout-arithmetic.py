#!/usr/bin/env python3
"""Check ToggleButtonGadget layout arithmetic uses signed dimensions."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleBG.c").read_text()

assert "((int)tb->rectangle.height - (int)onH) / 2" in source
assert "(short)((int)new_w->rectangle.width" in source
assert "(int)new_w->rectangle.height - (int)LabG_MarginTop" in source
assert "(int)tb->rectangle.height - (int)LabG_MarginTop" in source

print("ToggleButtonGadget layout geometry uses signed arithmetic")
