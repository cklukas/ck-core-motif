#!/usr/bin/env python3
"""Check ComboBox almost-geometry negotiation uses signed height deltas."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ComboBox.c").read_text()

assert "int diff, adjusted_edit_height;" in source
assert "abs((int) geom.height - (int) replygeom.height)" in source
assert "adjusted_edit_height = (int) ebH - diff;" in source
assert "(adjusted_edit_height > 0) ?" in source

print("ComboBox geometry negotiation is bounded")
