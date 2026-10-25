#!/usr/bin/env python3
"""Check FontSelector resolution scoring cannot overflow signed arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/FontS.c").read_text(encoding="utf-8")
assert "square_or_max" in source
assert "254LL * WidthOfScreen" in source
assert "WidthMMOfScreen(screen) > 0" in source
assert "xres = square_or_max(xres)" in source
assert "tdiff = sx > LLONG_MAX - sy ? LLONG_MAX : sx + sy" in source
print("PASS FontSelector resolution arithmetic is bounded")
