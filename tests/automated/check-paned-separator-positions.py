#!/usr/bin/env python3
"""Check Paned separator coordinates avoid mixed signed/unsigned arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Paned.c").read_text()

assert "int\toffset, sash_size, sash_pos, separator_pos;" in source
assert "separator_pos = (int) (*childP)->core.y" in source
assert "separator_pos = (int) (*childP)->core.x -" in source
assert "(int) (*childP)->core.x +" in source
assert "sep.y = (Position) separator_pos;" in source
assert "sep.x = (Position) separator_pos;" in source

print("Paned separator coordinates are signed")
