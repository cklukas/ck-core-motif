#!/usr/bin/env python3
"""Check ToggleButton pixmap redisplay clamps temporary text geometry."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleB.c").read_text(
    encoding="utf-8"
)
start = source.index("static void\nredisplayPixmap")
end = source.index("static void\nHandlePixmap", start)
body = source[start:end]

assert "static Position toggle_bounded_position(int64_t value);" in source
assert "Lab_TextRect_y(tb) = toggle_bounded_position(MAX(h, 0));" in body
assert "Lab_TextRect_height(tb) = toggle_bounded_dimension(onH);" in body
assert "Lab_TextRect_width(tb) = toggle_bounded_dimension(onW);" in body
print("PASS ToggleButton pixmap redisplay clamps text geometry")
