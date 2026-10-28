#!/usr/bin/env python3
"""Check ProgressRing drawing does not wrap center coordinates through Position."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ProgressRing.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(long long value)" in source
assert "int center_x, center_y" in source
assert "center_x = (int)ring->progress_ring.spacing" in source
assert "center_x - (int)diameter / 2" in source
assert "bounded_position((long long)center_x" in source
print("PASS ProgressRing center and text coordinates stay bounded")
