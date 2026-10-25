#!/usr/bin/env python3
"""Check LabelG background-box inset arithmetic stays signed."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "LabelG.c").read_text()

assert "background_box.width = (int) lw->rectangle.width -" in source
assert "background_box.height = (int) lw->rectangle.height -" in source
assert "(2 * (int) LabG_Highlight(lw))" in source
assert "background_box.width = lw->rectangle.width -" not in source
assert "background_box.height = lw->rectangle.height -" not in source

print("LabelG background-box spans are signed")
