#!/usr/bin/env python3
"""Ensure anchored popups widen coordinate arithmetic before clamping."""

from pathlib import Path

root = Path(__file__).parents[2]
for name, end_marker in (("AutoSuggestBox.c", "static void create_popup"),
                         ("Flyout.c", "static void create_popup"),
                         ("TeachingTip.c", "static void create_popup")):
    source = (root / "lib/Xm" / name).read_text(encoding="utf-8")
    start = source.index("static void position_popup")
    body = source[start:source.index(end_marker, start)]
    assert "long long x_pos, y_pos, max_x, max_y" in body
    assert "WidthOfScreen(screen)" in body and "HeightOfScreen(screen)" in body
    assert "x_pos > SHRT_MAX" in body and "y_pos > SHRT_MAX" in body
    assert "(Position)x_pos" in body and "(Position)y_pos" in body
print("PASS anchored popup placement uses wide, bounded coordinates")
