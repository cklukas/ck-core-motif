#!/usr/bin/env python3
"""Check ToggleButton pixmap centering clamps signed height differences."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleB.c").read_text(encoding="utf-8")
assert "h = ((int)XtHeight(tb) - (int)onH) / 2;" in source
assert "h = (XtHeight(tb) - onH) / 2;" not in source
assert "Lab_TextRect_y(tb) = MAX(h, 0);" in source
print("toggle-button pixmap centering uses signed arithmetic")
