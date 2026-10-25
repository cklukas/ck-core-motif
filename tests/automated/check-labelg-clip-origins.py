#!/usr/bin/env python3
"""Check LabelG narrow-layout clip origins use signed arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "LabelG.c").read_text()

assert "clip_rect.x = (short) ((int) lw->rectangle.x +" in source
assert "clip_rect.y = (short) ((int) lw->rectangle.y +" in source

print("LabelG clip origins are signed")
