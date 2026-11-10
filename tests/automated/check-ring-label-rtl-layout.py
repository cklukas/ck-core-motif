#!/usr/bin/env python3
"""Check Spinner and ProgressRing mirror their ring/label pair in RTL."""

from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
for name in ("Spinner.c", "ProgressRing.c"):
    source = (root / name).read_text()
    assert "XmDirectionMatchPartial(XmPrim_layout_direction" in source
    assert "ring_x = (int)" in source
    assert "- label_width" in source
print("PASS Spinner and ProgressRing mirror ring labels in RTL")
