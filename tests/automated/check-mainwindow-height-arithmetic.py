#!/usr/bin/env python3
"""Check MainWindow child resize arithmetic remains signed and bounded."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "MainW.c").read_text()

assert "int             parent_height, height_change;" in source
assert "((int) mb->core.height - (2 * (int) mb->core.border_width))" in source
assert "((int) w->core.height - (2 * (int) w->core.border_width))" in source
assert "(parent_height > 0) ? parent_height : 0" in source
assert "height_change = (int) newHeight - (int) OldHeight;" in source

print("MainWindow child height arithmetic is bounded")
