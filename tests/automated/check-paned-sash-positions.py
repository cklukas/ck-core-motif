#!/usr/bin/env python3
"""Check regular Paned sash coordinates remain signed."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Paned.c").read_text()

assert "sash_pos = (int) (*childP)->core.y" in source
assert "sash_pos = (int) (*childP)->core.x -" in source
assert "(int) (*childP)->core.x +" in source
assert "changes.y = (Position) sash_pos;" in source
assert "changes.x = (Position) sash_pos;" in source

print("Paned regular sash coordinates are signed")
