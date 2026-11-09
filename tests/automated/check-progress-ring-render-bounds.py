#!/usr/bin/env python3
"""Check ProgressRing keeps drawing arithmetic in its working domain."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ProgressRing.c").read_text()
assert "int center_x, center_y;" in source
assert "opening = diameter - 2 * thickness;" in source
assert "(unsigned long long)opening * (unsigned long long)opening" in source
print("PASS ProgressRing render arithmetic stays bounded before conversion")
