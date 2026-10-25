#!/usr/bin/env python3
"""Check shared shadow drawing rejects missing graphics contexts."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Draw.c").read_text()

assert "if(!d || !top_gc || !bottom_gc) return ;" in source

print("XmeDrawShadows rejects missing GCs")
