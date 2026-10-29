#!/usr/bin/env python3
"""Check Frame child X positions do not wrap through Position casts."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Frame.c").read_text()
assert "static Position\nFrameClampPosition(long long value)" in source
assert "if (value > SHRT_MAX)" in source
assert "if (value < SHRT_MIN)" in source
assert "childX = FrameClampPosition(child_x);" in source
assert "childX = FrameClampPosition((long long)shadowThickness +" in source
assert "childY = FrameClampPosition((long long)titleExtent +" in source
assert "childX = (Position) child_x;" not in source
print("PASS Frame child positions clamp before X11 conversion")
