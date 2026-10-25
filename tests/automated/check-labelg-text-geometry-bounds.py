#!/usr/bin/env python3
"""Check LabelG text right edge is computed in signed arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "LabelG.c").read_text()

assert "rightx = (int) newlw->rectangle.width -" in source
assert "(int) LabG_MarginRight(newlw);" in source
assert "rightx = newlw->rectangle.width -" not in source

print("LabelG text geometry is signed")
