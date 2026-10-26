#!/usr/bin/env python3
"""Ensure TextField timer-scroll edge arithmetic is widened and clamped."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text(
    encoding="utf-8"
)
start = source.index("CheckTimerScrolling(Widget w,")
end = source.index("\nstatic void \nRestorePrimaryHighlight", start)
region = source[start:end]

assert "int64_t margin_size" in region
assert "int64_t top_margin" in region
assert "int64_t text_bottom" in region
assert "int64_t edge_position" in region
assert "edge_position < SHRT_MIN" in region
assert "edge_position > SHRT_MAX" in region
assert "Dimension margin_size" not in region

print("TextField timer-scroll edges use widened clamped arithmetic")
