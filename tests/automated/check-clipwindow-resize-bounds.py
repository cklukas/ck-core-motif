#!/usr/bin/env python3
"""Check ClipWindow RTL resize arithmetic cannot wrap child positions."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ClipWindow.c").read_text()

assert "int d, new_x ;" in source
assert "(int)cw->clip_window.old_width - (int)child->core.x" in source
assert "new_x = (int)cw->core.width - d" in source
assert "new_x < INT16_MIN" in source
assert "new_x > INT16_MAX" in source
assert "(Position)new_x" in source

print("ClipWindow resize positions are bounded")
