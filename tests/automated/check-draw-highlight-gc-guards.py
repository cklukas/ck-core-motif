#!/usr/bin/env python3
"""Check shared highlight drawing rejects a missing graphics context."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Draw.c").read_text()

assert "if (!d || !gc || draw_width <= 0" in source

print("XmeDrawHighlight rejects a missing GC")
