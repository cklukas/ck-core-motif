#!/usr/bin/env python3
"""Check Label and LabelG layout uses signed geometry arithmetic."""

from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
label = (root / "Label.c").read_text()
labelg = (root / "LabelG.c").read_text()

assert "(int)newlw->core.height - (int)lp->margin_top" in label
assert "(short)((int)newlw->core.width" in label
assert "(int)newlw->rectangle.height - (int)LabG_MarginTop" in labelg
assert "(short)((int)newlw->rectangle.width" in labelg

print("Label layout geometry uses signed arithmetic")
