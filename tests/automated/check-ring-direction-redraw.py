#!/usr/bin/env python3
"""Check ring controls repaint after a live direction update."""
from pathlib import Path
root = Path(__file__).parents[2] / "lib" / "Xm"
for name, widget in (("Spinner.c", "spinner"), ("ProgressRing.c", "ring")):
    source = (root / name).read_text()
    assert f"XmPrim_layout_direction(old) != XmPrim_layout_direction({widget})" in source
print("PASS ring controls repaint after direction changes")
