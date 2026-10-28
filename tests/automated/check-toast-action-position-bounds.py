#!/usr/bin/env python3
"""Check Toast action-button origins stay within Position."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Toast.c").read_text()
assert "static Position clamp_position(long long value)" in source
assert "clamp_position(toast->core.width > aw + 8" in source
assert "(Position)(toast->core.width > aw + 8" not in source
print("PASS Toast action position is bounded")
